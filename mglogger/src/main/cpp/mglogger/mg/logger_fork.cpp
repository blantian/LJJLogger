/**
 * Description:
 * Created by lantian
 * Date： 2025/7/20
 * Time： 13:43
 *
 */
#include <sys/wait.h>
#include "logger_fork.h"

using namespace MGLogger;

LoggerFork::LoggerFork() {
    ALOGD("LoggerFork::LoggerFork - initialized");
    m_loggerQueue = std::make_shared<LoggerQueue>(500);
}

LoggerFork::~LoggerFork() {
    ALOGD("LoggerFork::~LoggerFork - shutting down");
    stop();
    if (forkThread) {
        SDL_WaitThread(forkThread, NULL);
        forkThread = nullptr;
    }
}

int LoggerFork::init() {
    return BaseLogger::init();
}

int LoggerFork::start() {
    ALOGD("LoggerFork::start - starting logger fork");
    s_running = true;
    forkThread = createForkThread();
    if (forkThread == nullptr) {
        ALOGE("LoggerFork::start - Failed to create fork thread");
        return MG_ERROR;
    }
    return MG_OK;
}

void LoggerFork::setBlackList(const std::list<std::string> &blackList) {
    ALOGD("LoggerFork::setBlackList - setting black list");
    m_blackList.clear();
    m_blackList.insert(blackList.begin(), blackList.end());
}

void LoggerFork::setLogcatArgs(const std::vector<std::string> &args) {
    ALOGD("LoggerFork::setLogcatArgs - setting logcat args");
    m_args_str.clear();
    m_args_str.reserve(args.size());
    for (const auto &arg : args) {
        m_args_str.emplace_back(arg);
    }
}

std::shared_ptr<MGMessage> LoggerFork::getMessage() {
    if (!messageQueue) {
        ALOGE("LoggerHook::getMessage - LoggerQueue not initialized");
        return nullptr;
    }
    return messageQueue->getMessage();
}


SDL_Thread *LoggerFork::createForkThread() {
    ALOGD("LoggerFork::createForkThread - creating fork thread");
    return SDL_CreateThreadEx(&_forkThread, &LoggerFork::forkThreadFunc, this,
                              "logger_fork_thread");
}

int LoggerFork::forkThreadFunc(void *arg) {
    ALOGD("LoggerFork::forkThreadFunc - starting fork thread");
    auto *loggerFork = static_cast<LoggerFork *>(arg);
    if (loggerFork) {
        return loggerFork->handleForkLogs();
    }
    ALOGE("LoggerFork::forkThreadFunc - fork thread end");
    return MG_ERROR;
}

int LoggerFork::handleForkLogs() {
    ALOGD("LoggerFork::handleForkLogs - handling fork logs");
    int pipe_fd[2];
    if (pipe(pipe_fd) == MG_ERROR) {
        ALOGE("LoggerFork::handleForkLogs - pipe creation failed: %s", strerror(errno));
        sendMessage(MG_LOGGER_STATUS_PIPE_CREATION_FAILED, "pipe creation failed");
        return MG_ERROR;
    }
    ALOGD("LoggerFork::handleForkLogs - pipe created");
    s_child_pid = fork();
    if (s_child_pid < 0) {
        ALOGE("LoggerFork::handleForkLogs - fork failed: %s", strerror(errno));
        sendMessage(MG_LOGGER_STATUS_FORK_FAILED, "fork failed");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        s_running = false;
        return MG_ERROR;
    } else if (s_child_pid == 0) {
        // 子进程
        ALOGD("LoggerFork::handleForkLogs - child process started");
        close(pipe_fd[0]); // 关闭读端
        dup2(pipe_fd[1], STDOUT_FILENO); // 重定向标准输出到管道
        dup2(pipe_fd[1], STDERR_FILENO); // 重定向标准错误到管道
        close(pipe_fd[1]); // 关闭写端

        m_args_str.emplace_back("logcat");
        m_args_str.emplace_back("-v");
        m_args_str.emplace_back("threadtime");
        for (const auto &tag: m_blackList) {
            if (!tag.empty()) {
                m_args_str.emplace_back(tag + ":S");
            }
        }
        std::vector<char *> argv;
        argv.reserve(m_args_str.size());
        for (auto &s: m_args_str) {
            argv.push_back(const_cast<char *>(s.c_str()));
        }
        argv.push_back(nullptr);
        const char *paths[] = {"/system/bin/logcat", "/system/xbin/logcat", nullptr};
        for (int i = 0; paths[i] != nullptr; ++i) {
            execv(paths[i], argv.data());
        }
        execvp("logcat", argv.data());
        ALOGE("LoggerFork::handleForkLogs - exec logcat failed: %s", strerror(errno));
        s_running = false;
        sendMessage(MG_LOGGER_STATUS_FORK_FAILED, "exec logcat failed");
        _exit(1);
    }

    // 父进程开始读取 log
    ALOGD("LoggerFork::handleForkLogs - parent start reading logs");
    close(pipe_fd[1]); // 关闭写端
    FILE *fp = fdopen(pipe_fd[0], "r"); // 打开管道读端
    if (!fp) {
        ALOGE("LoggerFork::handleForkLogs - failed to open pipe for logcat: %s", strerror(errno));
        sendMessage(MG_LOGGER_STATUS_PIPE_OPEN_FAILED, "failed to open pipe for logcat");
        close(pipe_fd[0]);
        s_running = false;
        return MG_ERROR;
    }
    ALOGD("LoggerFork::handleForkLogs - logcat pipe opened successfully pid=%d", s_child_pid);
    char buffer[MAX_MSG_LENGTH];
    while (s_running && fgets(buffer, sizeof(buffer), fp)) {
        // 处理读取到的日志
        if (strstr(buffer, MGLOGGER_LOG_TAG) != nullptr) {
            continue;
        }
        MGLog mgLog;
        mgLog.ts = getCurrentTimeMillis(); // 获取当前时间戳
        strncpy(mgLog.msg, buffer, MAX_MSG_LENGTH - 1);
        mgLog.msg[MAX_MSG_LENGTH - 1] = '\0'; // 确保字符串以 null 结尾
        writeLog(&mgLog, LOG_SRC_FORK); // 写入日志
    }
    fclose(fp);
    int status = 0;
    waitpid(s_child_pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        sendMessage(MG_LOGGER_STATUS_FORK_EXITED, "child process exited with error");
    }
    s_child_pid = -1;
    s_running = false;
    return MG_OK;
}

void LoggerFork::writeLog(MGLog *log, int sourceType) {
    if (!m_loggerQueue) {
        ALOGE("LoggerFork::writeLog - LoggerQueue not initialized");
        return;
    }
    // 将日志封装入队列
    BaseLogger::enqueue(log, sourceType);
}


int LoggerFork::dequeue(MGLog *log) {
    if (!m_loggerQueue) {
        ALOGE("LoggerFork::dequeue - LoggerQueue not initialized");
        return -1;
    }
    return m_loggerQueue->dequeue(log);
}

void LoggerFork::stop() {
    // 实现内容
    s_running = false;
    if (s_child_pid > 0) {
        kill(s_child_pid, SIGTERM);
        waitpid(s_child_pid, nullptr, 0);
        s_child_pid = -1;
    }
}


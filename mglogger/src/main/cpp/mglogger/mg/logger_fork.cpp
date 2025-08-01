/**
 * Description:
 * Created by lantian
 * Date： 2025/7/20
 * Time： 13:43
 *
 */
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <poll.h>
#include "logger_fork.h"

using namespace MGLogger;

static bool isLogcatAvailable() {
    const char *paths[] = {"/system/bin/logcat", "/system/xbin/logcat", nullptr};
    for (int i = 0; paths[i] != nullptr; ++i) {
        ALOGD("LoggerFork::isLogcatAvailable - checking path: %s isOk: %s", paths[i],
              access(paths[i], F_OK) == 0 ? "true" : "false");
        if (access(paths[i], X_OK) == 0) {
            return true;
        }
    }
    return false;
}

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
        return MG_LOGGER_FORK_THREAD_FAILED;
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
    if (!isLogcatAvailable()) {
        ALOGE("LoggerFork::handleForkLogs - logcat not available");
        sendMessage(MG_LOGGER_STATUS_LOGCAT_UNAVAILABLE, "logcat not available");
        s_running = false;
        return MG_ERROR;
    }
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
        sendMessage(MG_LOGGER_STATUS_FORK_STARTED, "child process started");
        close(pipe_fd[0]); // 关闭读端
        dup2(pipe_fd[1], STDOUT_FILENO); // 重定向标准输出到管道
        dup2(pipe_fd[1], STDERR_FILENO); // 重定向标准错误到管道
        close(pipe_fd[1]); // 关闭写端

        m_args_str.emplace_back("logcat");
        m_args_str.emplace_back("-v");
        m_args_str.emplace_back("threadtime");
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
    struct pollfd pfd{fileno(fp), POLLIN, 0};
    int pollRet;
    bool firstDataRead = false;
    while (s_running) {
        int timeout = firstDataRead ? -1 : LOGCAT_OUTPUT_TIMEOUT_MS;
        pollRet = poll(&pfd, 1, timeout);
        ALOGD("LoggerFork::handleForkLogs - poll returned %d, revents: %d", pollRet, pfd.revents);
        if (pollRet > 0 && (pfd.revents & POLLIN)) {
            if (!fgets(buffer, sizeof(buffer), fp)) {
                ALOGD("LoggerFork::handleForkLogs - fgets failed or EOF reached");
                break;
            }
            firstDataRead = true;
            if (strstr(buffer, MGLOGGER_LOG_TAG) != nullptr) {
                continue;
            }
            MGLog mgLog{};
            parseThreadTimeLine(buffer, &mgLog);
            writeLog(&mgLog, LOG_SRC_FORK);
        } else if (pollRet > 0 && (pfd.revents & (POLLHUP | POLLERR))) {
            ALOGE("LoggerFork::handleForkLogs - poll hangup or error");
            sendMessage(MG_LOGGER_STATUS_FORK_EXITED, "logcat hangup");
            break;
        } else if (pollRet == 0) { // timeout
            if (!firstDataRead) {
                ALOGE("LoggerFork::handleForkLogs - logcat no output, timeout");
                sendMessage(MG_LOGGER_STATUS_FORK_TIMEOUT, "fork timeout");
                fclose(fp);
                kill(s_child_pid, SIGTERM);
                waitpid(s_child_pid, nullptr, 0);
                s_child_pid = -1;
                s_running = false;
                return MG_ERROR;
            }
            // ignore timeout after first log has been read
            continue;
        } else if (pollRet < 0 && errno != EINTR) {
            ALOGE("LoggerFork::handleForkLogs - poll error: %s", strerror(errno));
            break;
        }
    }
    fclose(fp);
    int status = 0;
    waitpid(s_child_pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        sendMessage(MG_LOGGER_STATUS_FORK_EXITED, "child process exited with error");
        s_child_pid = -1;
        s_running = false;
        return MG_ERROR;
    }
    s_child_pid = -1;
    s_running = false;
    return MG_OK;
}

void LoggerFork::parseThreadTimeLine(const char *line, MGLog *out) {
    if (!line || !out) {
        return;
    }
    char date[16] = {0};
    char time[16] = {0};
    int pid = 0;
    long long tid = 0;
    char level = 'D';
    char tagBuf[MAX_TAG_LENGTH] = {0};
    char msgBuf[MAX_MSG_LENGTH] = {0};
    // 使用 sscanf 解析日志行
    int matched = sscanf(line,
                         "%15s %15s %d %lld %c %63[^:]: %2049[^\n]",
                         date, time, &pid, &tid, &level, tagBuf, msgBuf);
    if (matched >= 6) {
        out->tid = tid;
        size_t tagLen = strlen(tagBuf);
        // 去除 tagBuf 末尾的空格
        while (tagLen > 0 && isspace(static_cast<unsigned char>(tagBuf[tagLen - 1]))) {
            tagBuf[tagLen - 1] = '\0';
            --tagLen;
        }
        strncpy(out->tag, tagBuf, MAX_TAG_LENGTH - 1);
        out->tag[MAX_TAG_LENGTH - 1] = '\0';
        char finalMsg[MAX_MSG_LENGTH];
        snprintf(finalMsg, sizeof(finalMsg), "%c %s", level, msgBuf);
        strncpy(out->msg, finalMsg, MAX_MSG_LENGTH - 1);
        out->msg[MAX_MSG_LENGTH - 1] = '\0';
        switch (level) {
            case LOG_DEBUG:
                out->level = LEVEL_DEBUG;
                break;
            case LOG_INFO:
                out->level = LEVEL_INFO;
                break;
            case LOG_WARN:
                out->level = LEVEL_WARN;
                break;
            case LOG_ERROR:
                out->level = LEVEL_ERROR;
                break;
            case LOG_FATAL:
                out->level = LEVEL_FATAL;
                break;
            case LOG_VERBOSE:
                out->level = LEVEL_VERBOSE;
                break;
            default:
                out->level = LEVEL_UNKNOWN;
                break;
        }
    } else {
        out->tid = 0;
        out->tag[0] = '\0';
        strncpy(out->msg, line, MAX_MSG_LENGTH - 1);
        out->msg[MAX_MSG_LENGTH - 1] = '\0';
        out->level = LEVEL_UNKNOWN;
        ALOGE("LoggerFork::parseThreadTimeLine - Failed to parse log line: %s", line);
    }
    out->ts = utils::LoggerUtils::nowMs();
}

void LoggerFork::writeLog(MGLog *log, int sourceType) {
    if (!m_loggerQueue) {
        ALOGE("LoggerFork::writeLog - LoggerQueue not initialized");
        return;
    }
    if (filiterBlackList(log)){
        return;
    }
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
    s_running = false;
    if (s_child_pid > 0) {
        kill(s_child_pid, SIGTERM);
        waitpid(s_child_pid, nullptr, 0);
        s_child_pid = -1;
    }
}


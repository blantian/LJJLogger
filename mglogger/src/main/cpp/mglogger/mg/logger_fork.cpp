/**
 * Description:
 * Created by lantian
 * Date： 2025/7/20
 * Time： 13:43
 *
 */
#include "logger_fork.h"

using namespace MGLogger;

LoggerFork *LoggerFork::s_instance = nullptr;

LoggerFork::LoggerFork() {
    ALOGD("LoggerFork::LoggerFork - initialized");
    s_instance = this;
    m_loggerQueue = std::make_shared<LoggerQueue>(500);
    forkThread = createForkThread();
}

LoggerFork::~LoggerFork() {
    ALOGD("LoggerFork::~LoggerFork - shutting down");
    if (m_loggerQueue) {
        m_loggerQueue->abort();
        m_loggerQueue->clear();
    }
    m_loggerQueue = nullptr;
    s_instance = nullptr;
    if (forkThread) {
        SDL_WaitThread(forkThread, NULL);
        forkThread = nullptr;
    }
}

int LoggerFork::init() {
    ALOGD("LoggerFork::init - initializing fork logger");
    if (s_instance) {
        ALOGE("LoggerFork::init - LoggerFork already initialized");
        return MG_ERROR;
    }

}

void LoggerFork::setBlackList(const std::list<std::string> blackList) {
    ALOGD("LoggerFork::setBlackList - setting black list");
    m_blackList.clear();
    m_blackList.insert(blackList.begin(), blackList.end());
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
    return MG_ERROR;
}

int LoggerFork::handleForkLogs() {
    ALOGD("LoggerFork::handleForkLogs - handling fork logs");
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return MG_ERROR;
    }
    s_child_pid = fork();
    if (s_child_pid < 0) {
        ALOGE("LoggerFork::handleForkLogs - fork failed");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return MG_ERROR;
    } else if (s_child_pid == 0) {
        // child process
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        dup2(pipe_fd[1], STDERR_FILENO);
        close(pipe_fd[1]);

        // 执行日志处理逻辑
        int ret = execlp("logcat", "logcat", "-v", "time", nullptr);
        if (ret < 0) {
            ALOGE("LoggerFork::handleForkLogs - execlp failed: %s", strerror(errno));
            return MG_ERROR;
        }
        _exit(1); // 如果 execlp 失败，退出子进程
    }
}

void LoggerFork::writeLog(MGLog* log, int sourceType) {
    // 实现内容
}

void LoggerFork::enqueue(MGLog* log, int sourceType) {
    // 实现内容
}

int LoggerFork::dequeue(MGLog* log) {

}

void LoggerFork::stop() {
    // 实现内容
}


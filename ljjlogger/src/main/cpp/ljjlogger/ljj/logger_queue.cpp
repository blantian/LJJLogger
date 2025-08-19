
#include "logger_queue.h"
#include <memory>
/**
 * Description: MGLog 队列
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 *
 */

using namespace MGLogger;

LoggerQueue::LoggerQueue(size_t maxCapacity)
        : m_maxCapacity(maxCapacity), abort_request(0) {
    m_mutex = SDL_CreateMutex();
    if (!m_mutex) {
        ALOGE("Failed to create logger queue mutex");
    }
    m_cond = SDL_CreateCond();
    if (!m_cond) {
        ALOGE("Failed to create logger queue cond");
    }

}

LoggerQueue::~LoggerQueue() {
    // 确保队列停止并清空
    SDL_LockMutex(m_mutex);
    abort_request = 1;
    SDL_CondSignal(m_cond);
    logList.clear();
    SDL_UnlockMutex(m_mutex);
    // 销毁互斥锁和条件变量
    if (m_mutex) {
        SDL_DestroyMutex(m_mutex);
        m_mutex = nullptr;
    }
    if (m_cond) {
        SDL_DestroyCond(m_cond);
        m_cond = nullptr;
    }
}

/**
 * 入日志队列
 * @param log
 * @param sourceType
 */
void LoggerQueue::enqueue(const MGLog *log, LogSourceType sourceType) {
    SDL_LockMutex(m_mutex);
    if (abort_request) {
        // 队列已中止，不再接受新日志
        SDL_UnlockMutex(m_mutex);
        return;
    }
    if (logList.size() >= m_maxCapacity) {
        // 队列已满，丢弃新日志避免阻塞主线程
        SDL_UnlockMutex(m_mutex);
        ALOGW("LoggerQueue::enqueue - Queue full, dropping log (tid=%lld, tag=%s)",
              log->tid, log->tag);
        return;
    }
    // 创建新的日志节点并添加到队列
    auto node = std::make_shared<LoggerNode>();
    node->log = *log;
    node->source = sourceType;
    logList.push_back(node);
    // 通知消费者线程有新日志可用
    SDL_CondSignal(m_cond);
    SDL_UnlockMutex(m_mutex);
}

/**
 * 出日志队列
 * @param outLog 输出日志
 * @return 返回日志来源类型，-1 表示队列已中止且无日志可用
 */
int LoggerQueue::dequeue(MGLog *outLog) {
    SDL_LockMutex(m_mutex);
    // 等待日志到来或收到中止通知
    while (logList.empty() && !abort_request) {
        SDL_CondWait(m_cond, m_mutex);
    }
    // 若收到中止信号且队列已空，返回 -1 通知结束
    if (abort_request && logList.empty()) {
        SDL_UnlockMutex(m_mutex);
        ALOGW("LoggerQueue aborted, returning -1");
        return -1;
    }
    // 弹出一条日志并返回其来源类型
    auto node = logList.front();
    logList.pop_front();
    *outLog = node->log;
    LogSourceType sourceType = node->source;
    SDL_UnlockMutex(m_mutex);
    return static_cast<int>(sourceType);
}

/**
 * 中止日志队列
 */
void LoggerQueue::abort() {
    ALOGD("LoggerQueue::abort - Aborting logger queue");
    SDL_LockMutex(m_mutex);
    // 标记停止，并唤醒线程退出
    abort_request = 1;
    SDL_CondSignal(m_cond);
    SDL_UnlockMutex(m_mutex);
    ALOGD("LoggerQueue::abort - Logger queue aborted");
}

/**
 * 清空日志队列
 */
void LoggerQueue::clear() {
    SDL_LockMutex(m_mutex);
    logList.clear();
    SDL_CondSignal(m_cond);
    SDL_UnlockMutex(m_mutex);
}



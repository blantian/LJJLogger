/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 *
 */

#include "logger_queue.h"
#include <memory>

namespace MGLogger {

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
        SDL_LockMutex(m_mutex);
        abort();  // 确保队列停止
        clear();
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


    void LoggerQueue::enqueue(MGLog* log, int tag) {
        ALOGD("LoggerQueue::enqueue called with tag: %d", tag);
        SDL_LockMutex(m_mutex);
        std::shared_ptr<LoggerNode> node = std::make_shared<LoggerNode>();
        node->log = *log;
        node->tag = tag;
        if (logList.size() >= m_maxCapacity) {
            // 队列已满，丢弃新任务以避免阻塞
            SDL_UnlockMutex(m_mutex);
            ALOGW("LoggerQueue is full, dropping new log entry");
            return;
        }
        // 将任务添加到队列
        logList.push_back(node);
        SDL_CondSignal(m_cond);
        SDL_UnlockMutex(m_mutex);
    }

    int LoggerQueue::dequeue(MGLog *log) {
        SDL_LockMutex(m_mutex);
        // 等待直到有日志可用
        while (logList.empty() && !abort_request) {
            SDL_CondWaitTimeout(m_cond, m_mutex, 20); // 等待10毫秒
        }
        // 如果收到停止信号且队列已空，返回-1
        if (abort_request && logList.empty()) {
            SDL_UnlockMutex(m_mutex);
            ALOGW("LoggerQueue aborted, returning -1");
            return -1;
        }
        // 取出一个日志任务
        auto node = logList.front();
        logList.pop_front();
        *log = node->log;
        SDL_UnlockMutex(m_mutex);
        return node->tag;
    }


    void LoggerQueue::abort() {
        SDL_LockMutex(m_mutex);
        // 标记停止，并唤醒线程退出
        abort_request = 1;
        SDL_CondSignal(m_cond);
        SDL_UnlockMutex(m_mutex);
    }

    void LoggerQueue::clear() {
        SDL_LockMutex(m_mutex);
        logList.clear();
        SDL_CondSignal(m_cond);
        SDL_UnlockMutex(m_mutex);
    }

}



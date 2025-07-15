/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 *
 * A simple single-threaded task queue used to serialize
 * access to clogan APIs. All public methods are thread-safe.
 */


#include "logger_queue.h"
#include "sdl_log.h"

logger_queue::logger_queue(size_t maxCapacity)
        :m_worker_tid(nullptr), m_mutex(nullptr), m_cond(nullptr),
           m_maxCapacity(maxCapacity), m_stop(false){
    m_mutex = SDL_CreateMutex();
    if (!m_mutex) {
        ALOGE("Failed to create logger queue mutex");
    }
    m_cond = SDL_CreateCond();
    if (!m_cond) {
        ALOGE("Failed to create logger queue cond");
    }

    m_worker_tid = SDL_CreateThreadEx(&m_worker, &threadFunc, this, "logger_queue_worker");
    if (!m_worker_tid){
        ALOGE("Failed to create logger queue thread");
    }
}

logger_queue::~logger_queue() {
    stop();
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


int logger_queue::threadFunc(void* data) {
    auto* self = static_cast<logger_queue*>(data);
    return self ? self->run() : 0;
}

void logger_queue::enqueue(const std::function<void()>& task) {
    SDL_LockMutex(m_mutex);
    if (m_tasks.size() >= m_maxCapacity) {
        // 队列已满，丢弃新任务以避免阻塞
        SDL_UnlockMutex(m_mutex);
        return;
    }
    // 将任务添加到队列
    bool wasEmpty = m_tasks.empty();
    m_tasks.push(task);
    // 唤醒日志线程（如果线程在等待）
    if (wasEmpty) {
        SDL_CondSignal(m_cond);
    }
    SDL_UnlockMutex(m_mutex);
}

void logger_queue::stop() {
    SDL_LockMutex(m_mutex);
    // 标记停止，并唤醒线程退出
    m_stop = true;
    SDL_CondSignal(m_cond);
    SDL_UnlockMutex(m_mutex);
    // 等待线程结束
    if (m_worker_tid) {
        SDL_WaitThread(m_worker_tid, nullptr);
        m_worker_tid = nullptr;
    }
}


int logger_queue::run() {
    SDL_LockMutex(m_mutex);
    // 日志线程主循环
    while (true) {
        // 等待任务或退出信号
        while (!m_stop && m_tasks.empty()) {
            SDL_CondWait(m_cond, m_mutex);
        }
        // 如果收到停止信号且队列已空，退出循环
        if (m_stop && m_tasks.empty()) {
            SDL_UnlockMutex(m_mutex);
            break;
        }
        // 取出一个日志任务执行
        std::function<void()> task = m_tasks.front();
        m_tasks.pop();
        SDL_UnlockMutex(m_mutex);
        // 执行日志写入任务（调用clogan接口）
        task();
        SDL_LockMutex(m_mutex);
    }
    // 线程退出
    return 0;
}


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

logger_queue::logger_queue(size_t max_queue_size)
        : m_running(true), m_max_size(max_queue_size),
            m_queue_mutex(SDL_CreateMutex()),
            m_cv_not_empty(SDL_CreateCond()),
            m_cv_not_full(SDL_CreateCond()){
    m_worker_tid = SDL_CreateThreadEx(&m_worker, &workerTh, this, "logger_queue_worker");
}

logger_queue::~logger_queue() {
    stop();
}


int logger_queue::workerTh(void* arg) {
    auto* queue = static_cast<logger_queue*>(arg);
    queue->loop();
    return 0;
}

void logger_queue::enqueue(const std::function<void()>& task) {
    if (!m_running) return;

    SDL_LockMutex(m_queue_mutex);
    while (m_queue.size() >= m_max_size && m_running) {
        SDL_CondWait(m_cv_not_full, m_queue_mutex);
    }

    if (m_running) {
        m_queue.push(task);
        SDL_CondSignal(m_cv_not_empty);
    }
    SDL_UnlockMutex(m_queue_mutex);
}

void logger_queue::stop() {
    SDL_LockMutex(m_queue_mutex);
    m_running = false;
    SDL_CondBroadcast(m_cv_not_empty);
    SDL_CondBroadcast(m_cv_not_full);
    SDL_UnlockMutex(m_queue_mutex);

    if (m_worker_tid) {
        SDL_WaitThread(m_worker_tid, nullptr);
        m_worker_tid = nullptr;
    }
    if (m_queue_mutex) SDL_DestroyMutex(m_queue_mutex);
    if (m_cv_not_empty) SDL_DestroyCond(m_cv_not_empty);
    if (m_cv_not_full) SDL_DestroyCond(m_cv_not_full);
}

void logger_queue::loop() {
    while (true) {
        task_t task;
        {

        }
        task();
    }
}


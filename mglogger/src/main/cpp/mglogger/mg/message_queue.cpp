/**
 * Description:
 * Created by lantian 
 * Date: 2025/7/14
 * Time: 15:56
 *
 */
#include "message_queue.h"

message_queue::message_queue(size_t cap)
        : m_queue_mutex(SDL_CreateMutex()),
          m_cv_not_empty(SDL_CreateCond()),
          m_cv_not_full(SDL_CreateCond()),
          m_worker_tid(nullptr),
          m_running(true),
          m_capacity(cap),
          m_callback(nullptr) {
    m_worker_tid = SDL_CreateThreadEx(&_m_worker_thread, &workerEntry, this, "logan_status_worker");
}

message_queue::~message_queue() { stop(); }

void message_queue::push(int code, const char *type) {
    if (!m_running) return;
    SDL_LockMutex(m_queue_mutex);
    while (m_queue.size() >= m_capacity && m_running)
        SDL_CondWait(m_cv_not_full, m_queue_mutex);

    if (m_running) {
        m_queue.push({code, type ? type : ""});
        SDL_CondSignal(m_cv_not_empty);
    }
    SDL_UnlockMutex(m_queue_mutex);
}

void message_queue::setCallback(StatusCallback cb) {
    SDL_LockMutex(m_queue_mutex);
    m_callback = cb;
    SDL_UnlockMutex(m_queue_mutex);
}

void message_queue::stop() {
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

int message_queue::workerEntry(void *arg) {
    static_cast<message_queue *>(arg)->loop();
    return 0;
}

void message_queue::loop() {
    while (true) {
        LoganStatus st;
        SDL_LockMutex(m_queue_mutex);
        while (m_queue.empty() && m_running)
            SDL_CondWait(m_cv_not_empty, m_queue_mutex);

        if (!m_running && m_queue.empty()) {
            SDL_UnlockMutex(m_queue_mutex);
            break;
        }
        st = std::move(m_queue.front());
        m_queue.pop();
        SDL_CondSignal(m_cv_not_full);
        SDL_UnlockMutex(m_queue_mutex);

        // 调用回调
        if (m_callback) m_callback(st.code, st.type.c_str());
    }
}


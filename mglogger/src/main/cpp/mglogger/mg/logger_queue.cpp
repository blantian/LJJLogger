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

logger_queue::logger_queue() : m_worker(&logger_queue::loop, this) {}

logger_queue::~logger_queue() {
    stop();
}

void logger_queue::stop() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_running = false;
    }
    m_cv_not_empty.notify_all();
    m_cv_not_full.notify_all();
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

int logger_queue::dispatch(const std::function<void()> &task) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_running) {
        return -1;
    }
    while (m_queue.size() >= m_capacity && m_running) {
        m_cv_not_full.wait(lock);
    }
    if (!m_running) {
        return -1;
    }
    m_queue.emplace(task);
    m_cv_not_empty.notify_one();
    return 0;
}

int logger_queue::dispatch_sync(const std::function<int()> &task) {
    auto p = std::make_shared<std::promise<int>>();
    auto f = p->get_future();
    int ret = dispatch([task, p]() { p->set_value(task()); });
    if (ret != 0) {
        return ret;
    }
    return f.get();
}

void logger_queue::dispatch_sync_void(const std::function<void()> &task) {
    auto p = std::make_shared<std::promise<void>>();
    auto f = p->get_future();
    dispatch([task, p]() {
        task();
        p->set_value();
    });
    f.get();
}

void logger_queue::loop() {
    while (true) {
        task_t task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv_not_empty.wait(lock, [this]() { return !m_queue.empty() || !m_running; });
            if (!m_running && m_queue.empty()) {
                break;
            }
            task = std::move(m_queue.front());
            m_queue.pop();
            m_cv_not_full.notify_one();
        }
        task();
    }
}


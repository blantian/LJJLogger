/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 *
 * A simple single-threaded task queue used to serialize
 * access to clogan APIs. All public methods are thread-safe.
 */


#include "message_queue.h"

message_queue::message_queue() : m_worker(&message_queue::loop, this) {}

message_queue::~message_queue() {
    stop();
}

void message_queue::stop() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_running = false;
    }
    m_cv.notify_all();
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

int message_queue::dispatch(const std::function<int()> &task) {
    auto p = std::make_shared<std::promise<int>>();
    auto f = p->get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_running) {
            return -1;
        }
        m_queue.push([task, p]() { p->set_value(task()); });
    }
    m_cv.notify_one();
    return f.get();
}

void message_queue::dispatch_void(const std::function<void()> &task) {
    auto p = std::make_shared<std::promise<void>>();
    auto f = p->get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_running) {
            return;
        }
        m_queue.push([task, p]() {
            task();
            p->set_value();
        });
    }
    m_cv.notify_one();
    f.get();
}

void message_queue::loop() {
    while (true) {
        task_t task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() { return !m_queue.empty() || !m_running; });
            if (!m_running && m_queue.empty()) {
                break;
            }
            task = std::move(m_queue.front());
            m_queue.pop();
        }
        task();
    }
}


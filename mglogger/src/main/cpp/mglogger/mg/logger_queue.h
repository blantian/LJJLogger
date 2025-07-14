/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 15:16
 */


#ifndef MGLOGGER_LOGGER_QUEUE_H
#define MGLOGGER_LOGGER_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>

class logger_queue {
public:
    logger_queue();
    ~logger_queue();

    // Asynchronously enqueue a task. Returns 0 on success or -1 if the queue
    // has been stopped.
    int dispatch(const std::function<void()> &task);

    // Schedule a task and wait for its result. This is used for operations that
    // require a return value.
    int dispatch_sync(const std::function<int()> &task);

    // Schedule a void task and block until completion.
    void dispatch_sync_void(const std::function<void()> &task);

    void stop();

private:
    using task_t = std::function<void()>;
    void loop();

    std::queue<task_t> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv_not_empty;
    std::condition_variable m_cv_not_full;
    bool m_running{true};
    std::thread m_worker;
    const size_t m_capacity = 1024;
};

#endif //MGLOGGER_LOGGER_QUEUE_H

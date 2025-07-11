/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 15:16
 */


#ifndef MGLOGGER_MESSAGE_QUEUE_H
#define MGLOGGER_MESSAGE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>

class message_queue {
public:
    message_queue();
    ~message_queue();

    // Schedule a task returning an int result. The task will be executed
    // on the internal worker thread and the result is returned to the caller.
    // When the queue has been stopped, this returns -1 without executing
    // the task.
    int dispatch(const std::function<int()> &task);

    // Schedule a void task. The call blocks until the task has been
    // executed. When the queue has been stopped the task is ignored.
    void dispatch_void(const std::function<void()> &task);
    void stop();

private:
    using task_t = std::function<void()>;
    void loop();

    std::queue<task_t> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_running{true};
    std::thread m_worker;
};

#endif //MGLOGGER_MESSAGE_QUEUE_H

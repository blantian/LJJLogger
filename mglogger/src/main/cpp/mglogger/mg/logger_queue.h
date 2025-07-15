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
#include "sdl_mutex.h"
#include "sdl_thread.h"
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>

class logger_queue {
public:
    logger_queue(size_t max_queue_size = 500);

    ~logger_queue();

    void enqueue(const std::function<void()> &task);

    void stop();

private:
    using task_t = std::function<void()>;
    static int workerTh(void* arg);   // SDL 线程入口
    void loop();

    std::queue<task_t> m_queue;
    SDL_mutex *m_queue_mutex{nullptr};
    SDL_cond *m_cv_not_empty{nullptr};
    SDL_cond *m_cv_not_full{nullptr};
    bool m_running;
    const size_t m_max_size;
    SDL_Thread *m_worker_tid{nullptr};
    SDL_Thread m_worker{};
};

#endif //MGLOGGER_LOGGER_QUEUE_H

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
    explicit logger_queue(size_t maxCapacity = 500);

    ~logger_queue();

    void enqueue(const std::function<void()> &task);

    void stop();

private:
    // 禁止拷贝
    logger_queue(const logger_queue&) = delete;
    logger_queue& operator=(const logger_queue&) = delete;
    static int threadFunc(void* arg);   // SDL 线程入口
    int run();
private:
    SDL_mutex* m_mutex;
    SDL_cond* m_cond;
    std::queue<std::function<void()>> m_tasks;
    const size_t m_maxCapacity;
    bool m_stop;
    SDL_Thread *m_worker_tid{nullptr};
    SDL_Thread m_worker{};
};

#endif //MGLOGGER_LOGGER_QUEUE_H

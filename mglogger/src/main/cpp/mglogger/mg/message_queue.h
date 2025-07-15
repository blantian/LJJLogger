#ifndef MGLOGGER_MESSAGE_QUEUE_H
#define MGLOGGER_MESSAGE_QUEUE_H

#include <queue>
#include <string>
#include "sdl_mutex.h"
#include "sdl_thread.h"

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 15:16
 */


class message_queue {
public:
    explicit message_queue(size_t capacity = 100);
    ~message_queue();

    // 生产：推送状态；队列满则阻塞
    void push(int code, const char* type);

    // 设置本地回调函数指针（可选，用于C层回调）
    void setCallback(void (*callback)(int, const char*));

    // 关闭并回收线程
    void stop();

private:

    // 禁止拷贝
    message_queue(const message_queue&) = delete;
    message_queue& operator=(const message_queue&) = delete;

    static int threadFunc(void* arg);   // SDL 线程入口
    int       run();                   // 实际消费者循环

    struct StatusEvent {
        int code;
        std::string type;
        StatusEvent(int c, const std::string& t) : code(c), type(t) {}
    };

private:
    std::queue<StatusEvent> m_events;
    SDL_mutex* m_mutex;
    SDL_cond* m_cond;
    SDL_Thread*             m_worker_tid;
    SDL_Thread              _m_worker_thread{};
    bool m_stop;
    const size_t            m_capacity;
    void (*m_callback)(int, const char*);
};

#endif // MGLOGGER_MESSAGE_QUEUE_H

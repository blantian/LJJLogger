#ifndef MGLOGGER_MESSAGE_QUEUE_H
#define MGLOGGER_MESSAGE_QUEUE_H

#include <queue>
#include <string>
#include "sdl_mutex.h"
#include "sdl_thread.h"

struct LoganStatus {
    int         code;
    std::string type;          // e.g. "clogan_init" / "clogan_open" / "clogan_write"
};

// 回调函数签名：code、type
using StatusCallback = void (*)(int, const char*);

class message_queue {
public:
    explicit message_queue(size_t capacity = 100);
    ~message_queue();

    // 生产：推送状态；队列满则阻塞
    void push(int code, const char* type);

    // 注册 C 回调
    void setCallback(StatusCallback cb);

    // 关闭并回收线程
    void stop();

private:
    static int workerEntry(void* arg);   // SDL 线程入口
    void       loop();                   // 实际消费者循环

    std::queue<LoganStatus> m_queue;
    SDL_mutex*              m_queue_mutex;
    SDL_cond*               m_cv_not_empty;
    SDL_cond*               m_cv_not_full;
    SDL_Thread*             m_worker_tid;
    SDL_Thread              _m_worker_thread{};
    bool                    m_running;
    const size_t            m_capacity;
    StatusCallback          m_callback;
};

#endif // MGLOGGER_MESSAGE_QUEUE_H

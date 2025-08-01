/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 15:16
 */


#ifndef MGLOGGER_LOGGER_QUEUE_H
#define MGLOGGER_LOGGER_QUEUE_H

#include "list"

#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#include "sdl_thread.h"
#include "sdl_mutex.h"
#ifdef __cplusplus
}
#endif

constexpr size_t MAX_TAG_LENGTH = 64;
constexpr size_t MAX_MSG_LENGTH = 2050;

// 日志条目结构
typedef struct Log {
    long long tid;                   // 线程 ID
    char tag[MAX_TAG_LENGTH];        // 日志标签
    char msg[MAX_MSG_LENGTH];        // 日志内容
    uint64_t ts;                    // 时间戳（毫秒）
    int level;                      // 日志级别
} MGLog;

namespace MGLogger {

    // 日志来源类型，用于标识日志来自哪个 hook 函数
    enum LogSourceType {
        LOG_SRC_UNKNOWN  = -1,
        LOG_SRC_PRINT    = 0,
        LOG_SRC_WRITE    = 1,
        LOG_SRC_VPRINT   = 2,
        LOG_SRC_BUF_WRITE= 3,
        LOG_SRC_ASSERT   = 4,
        LOG_SRC_FORK     = 5,
    };

    class LoggerNode;

    class LoggerQueue {
    public:
        explicit LoggerQueue(size_t maxCapacity = 1000);
        ~LoggerQueue();

        // 将日志加入队列（非阻塞；若队列已满则直接丢弃）
        void enqueue(const MGLog *log, LogSourceType sourceType);
        // 从队列取出一条日志；返回日志来源类型，若返回 -1 表示队列结束（中止）
        int dequeue(MGLog *outLog);

        inline size_t getSize() const { return logList.size(); }
        inline bool isAborted() const { return abort_request != 0; }

        // 中止队列（通知等待的消费者线程退出）
        void abort();
        // 清空队列中剩余的日志
        void clear();

    private:


    private:
        SDL_mutex *m_mutex{nullptr};
        SDL_cond *m_cond{nullptr};
        std::list<std::shared_ptr<LoggerNode>> logList;
        const size_t m_maxCapacity;
        int abort_request{0};
    };

    class LoggerNode {
    public:
        LoggerNode() = default;
        ~LoggerNode() = default;

    private:
        MGLog log;
        LogSourceType source;                // 日志来源类型
        friend class LoggerQueue;
    };
}

#endif //MGLOGGER_LOGGER_QUEUE_H

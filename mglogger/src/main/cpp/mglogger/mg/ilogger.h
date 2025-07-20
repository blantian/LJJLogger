//
// Created by sky blue on 2025/7/17.
//

#ifndef MGLOGGER_ILOGGER_H
#define MGLOGGER_ILOGGER_H

#include <memory>
#include <unistd.h>
#include "logger_queue.h"
#include "string"

#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#include "sdl_android_jni.h"
#ifdef __cplusplus
}
#endif

namespace MGLogger {

    class ILogger {
    public:
        static std::shared_ptr<ILogger> CreateLogger(int type);
        virtual void init() = 0;

        virtual void writeLog(MGLog *log, int tag) = 0;

        virtual void enqueue(MGLog *log, int tag) = 0;

        virtual int dequeue(MGLog *log) = 0;

        virtual void stop() = 0;

        virtual void setBlackList(const std::vector<std::string> &blackList);

        // 工具函数：获取当前线程 ID
        static inline pid_t my_tid() {
#ifdef __ANDROID__
            return gettid();
#else
            return (pid_t) syscall(SYS_gettid);
#endif
        }
        // 工具函数：获取当前时间（毫秒）
        static inline long long getCurrentTimeMillis() {
#if defined(CLOCK_REALTIME)
            struct timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            return (long long) ts.tv_sec * 1000LL + ts.tv_nsec / 1000000;
#else
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return (long long) tv.tv_sec * 1000LL + tv.tv_usec / 1000;
#endif
        }
        // 工具函数：获取当前进程（主线程）ID
        static inline pid_t my_pid() {
            return getpid();
        }

    protected:
        std::shared_ptr<LoggerQueue> m_loggerQueue{nullptr};
        static std::shared_ptr<ILogger> logger;
        static std::vector<std::string> m_blackList; // 黑名单列表
    };
}

#endif //MGLOGGER_ILOGGER_H

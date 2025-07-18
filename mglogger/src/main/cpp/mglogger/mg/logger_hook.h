//
// Created by sky blue on 2025/7/16.
//

#ifndef MGLOGGER_LOGGER_HOOK_H
#define MGLOGGER_LOGGER_HOOK_H


#include <unistd.h>
#include "logger_queue.h"
#include <memory>
#include "ilogger.h"
#include <android/log.h>
#include "logger_config.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "xhook.h"
#include "sdl_mutex.h"
#include "sdl_thread.h"
#include "sdl_log.h"
#ifdef __cplusplus
}
#endif


namespace MGLogger {

    static int (*orig_log_print)(int, const char *, const char *, ...) = nullptr;

    static int (*orig_log_write)(int, const char *, const char *) = nullptr;

    static int (*orig_log_buf_write)(int, int, const char *, const char *) = nullptr;

    static int (*orig_log_vprint)(int, const char *, const char *, va_list) = nullptr;

    static void (*orig_log_assert)(const char *, const char *, const char *, ...) = nullptr;

    class LoggerHook : public ILogger {

    public:
        LoggerHook();

        ~LoggerHook();

        void init() override;

        int dequeue(MGLog *log) override;

    private:

        static int hookLogPrint(int prio, const char *tag, const char *fmt, ...);

        static int hookLogBufWrite(int bufID, int prio, const char *tag, const char *text);

        static int hookLogWrite(int prio, const char *tag, const char *buf);

        static int hookLogVPrint(int prio, const char *tag, const char *fmt, va_list ap);

        static void hookLogAssert(const char *cond, const char *file, const char *func, ...);

    private:
        void writeLog(MGLog *log, int tag) override;

        void enqueue(MGLog *log, int tag) override;

        static inline pid_t my_tid() {
#ifdef __ANDROID__
            return gettid();
#else
            return (pid_t)syscall(SYS_gettid);
#endif
        }

        static inline long long getCurrentTimeMillis() {
#if defined(CLOCK_REALTIME)
            struct timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);     // 纳秒级
            return (long long) ts.tv_sec * 1000LL +
                   ts.tv_nsec / 1000000;
#else
            struct timeval tv;
    gettimeofday(&tv, nullptr);             // 微秒级
    return (long long)tv.tv_sec * 1000LL +
           tv.tv_usec / 1000;
#endif
        }

        static inline pid_t my_pid() {
            return getpid();
        }

    private:
        static LoggerHook *s_instance;
        std::shared_ptr<LoggerQueue> m_loggerQueue;
    };
}

#endif //MGLOGGER_LOGGER_HOOK_H

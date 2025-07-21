//
// Created by sky blue on 2025/7/16.
//

#ifndef MGLOGGER_LOGGER_HOOK_H
#define MGLOGGER_LOGGER_HOOK_H


#include "logger_queue.h"
#include <memory>
#include "ilogger.h"
#include <android/log.h>
#include "logger_status.h"
#include <set>

#ifdef __cplusplus
extern "C" {
#endif
#include "xhook.h"
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

        int init() override;

        int dequeue(MGLog *log) override;
        // 停止 Hook（中止队列消费）
        void stop() override;

        void setBlackList(std::list<std::string> blackList) override;

    private:

        static int hookLogPrint(int prio, const char *tag, const char *fmt, ...);

        static int hookLogBufWrite(int bufID, int prio, const char *tag, const char *text);

        static int hookLogWrite(int prio, const char *tag, const char *buf);

        static int hookLogVPrint(int prio, const char *tag, const char *fmt, va_list ap);

        static void hookLogAssert(const char *cond, const char *file, const char *func, ...);

        // 将日志加入队列
        void enqueue(MGLog *log, int sourceType) override;
        // 写入日志至队列（内部调用 enqueue）
        void writeLog(MGLog *log, int sourceType) override;

    private:
        std::set<std::string> m_blackList;
        static LoggerHook *s_instance;
    };
}

#endif //MGLOGGER_LOGGER_HOOK_H

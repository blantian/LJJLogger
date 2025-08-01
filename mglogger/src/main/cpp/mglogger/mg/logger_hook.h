//
// Created by sky blue on 2025/7/16.
//

#ifndef MGLOGGER_LOGGER_HOOK_H
#define MGLOGGER_LOGGER_HOOK_H


#include "logger_queue.h"
#include <memory>
#include "logger_base.h"
#include <android/log.h>
#include "logger_common.h"
#include <set>

#ifdef __cplusplus
extern "C" {
#endif
#include "external/hook/xhook.h"
#ifdef __cplusplus
}
#endif


namespace MGLogger {

    static int (*orig_log_print)(int, const char *, const char *, ...) = nullptr;

    static int (*orig_log_write)(int, const char *, const char *) = nullptr;

    static int (*orig_log_buf_write)(int, int, const char *, const char *) = nullptr;

    static int (*orig_log_vprint)(int, const char *, const char *, va_list) = nullptr;

    static void (*orig_log_assert)(const char *, const char *, const char *, ...) = nullptr;

    class LoggerHook : public BaseLogger {

    public:
        LoggerHook();

        ~LoggerHook() override;

        int init() override;

        int start() override {
            return MG_OK;
        };

        int dequeue(MGLog *log) override;

        void setBlackList(const std::list<std::string> &blackList) override;

        void setLogcatArgs(const std::vector<std::string> &args) override;

        std::shared_ptr<MGMessage> getMessage() override;

    private:

        static int hookLogPrint(int prio, const char *tag, const char *fmt, ...);

        static int hookLogBufWrite(int bufID, int prio, const char *tag, const char *text);

        static int hookLogWrite(int prio, const char *tag, const char *buf);

        static int hookLogVPrint(int prio, const char *tag, const char *fmt, va_list ap);

        static void hookLogAssert(const char *cond, const char *file, const char *func, ...);

        // 写入日志至队列（内部调用 enqueue）
        void writeLog(MGLog *log, int sourceType) override;


        inline void sendMessage(int what) {
            messageQueue->sendMessage(what);
        }

        inline void sendMessage(int what, const char *cmd) {
            messageQueue->sendMessage(what, cmd);
        }

        inline void sendMessage(const std::shared_ptr<MGMessage>& msg){
            messageQueue->sendMessage(msg);
        }


    private:
        static LoggerHook *s_instance;
    };
}

#endif //MGLOGGER_LOGGER_HOOK_H

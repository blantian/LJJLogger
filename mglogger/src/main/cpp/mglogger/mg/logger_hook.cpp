/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/4
 * Time： 23:58
 *
 * 1. 上层无感知切换
 * 2. 性能对比 ，压缩比对比
 *
 */


#include "logger_hook.h"

namespace MGLogger {

    LoggerHook::LoggerHook() : m_loggerQueue(std::make_shared<LoggerQueue>(1000)) {
        ALOGD("LoggerHook() called %s", __func__);
        s_instance = this;
    }

    LoggerHook::~LoggerHook() {
        m_loggerQueue->abort();
        m_loggerQueue->clear();
    }

    LoggerHook *LoggerHook::s_instance = nullptr;

    void LoggerHook::init() {
        ALOGD("LoggerHook::init() called, %s", __func__);
        if (orig_log_print || orig_log_write || orig_log_buf_write || orig_log_vprint) {
            ALOGD("LoggerHook::init() already initialized, skipping");
            return;
        }

#if OPEN_WRITE
        ALOGD("LoggerHook::init() called");
        int w_code = xhook_register(".*\\.so$", LOG_WRITE, (void *) hookLogWrite,
                                    (void **) &orig_log_write);
        if (w_code != 0) {
            ALOGE("hook_register failed for LOG_WRITE, code=%d", w_code);
        } else {
            ALOGD("hook_register succeeded for LOG_WRITE");
        }
#endif

#if OPEN_PRINT
        int p_code = xhook_register(".*\\.so$", LOG_PRINT, (void *) hookLogPrint,
                                    (void **) &orig_log_print);

        if (p_code != 0) {
            ALOGE("hook_register failed for LOG_PRINT, code=%d", p_code);
        } else {
            ALOGD("hook_register succeeded for LOG_PRINT");
        }
#endif

#if OPEN_ASSERT
        int vp_code = xhook_register(".*\\.so$", LOG_VPRINT, (void *) hookLogVPrint,
                                     (void **) &orig_log_vprint);

        if (vp_code != 0) {
            ALOGE("hook_register failed for LOG_VPRINT, code=%d", vp_code);
        } else {
            ALOGD("hook_register succeeded for LOG_VPRINT");
        }
#endif

#if OPEN_BUF_WRITE
        int lbw_code = xhook_register(".*\\.so$", LOG_BUF_WRITE, (void *) hookLogBufWrite,
                                      (void **) &orig_log_buf_write);

        if (lbw_code != 0) {
            ALOGE("hook_register failed for LOG_BUF_WRITE, code=%d", lbw_code);
        } else {
            ALOGD("hook_register succeeded for LOG_BUF_WRITE");
        }
#endif

        xhook_refresh(1);
        ALOGD("LoggerHook::init() completed, original functions hooked");
    }

#if OPEN_WRITE
    int LoggerHook::hookLogWrite(int prio, const char *tag, const char *buf) {
        MGLog log{};
        log.tid = my_tid();
        log.tag = tag ? tag : "unknown";
        strncpy(log.msg, buf ? buf : "", sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();

        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG,
                           LOG_WRITE,
                           "hook_log_write: prio=%d, tag=%s, buf=%s, tid=%lld, ts=%lld",
                           prio, log.tag, log.msg, log.tid, log.ts);
        }

        int ret = 0;
        if (orig_log_write) {
            ret = orig_log_write(prio, tag ? tag : "NULL", buf ? buf : "");
        }
        return ret;
    }
#endif

#if OPEN_PRINT

    int LoggerHook::hookLogPrint(int prio, const char *tag, const char *fmt, ...) {
        char msgBuf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, ap);
        va_end(ap);
        MGLog log{};
        log.tid = my_tid();
        log.tag = tag ? tag : "unknown";
        strncpy(log.msg, msgBuf, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
//        s_instance -> writeLog(&log, PRINT);
        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG,
                           LOG_PRINT,
                           "hook_log_print: prio=%d, tag=%s ,tid=%lld, msg=%s ,ts=%lld",
                           prio, log.tag, log.tid, log.msg, log.ts);
        }

        int ret = 0;
        if (orig_log_print) {
            ret = orig_log_print(prio, tag ? tag : "NULL", "%s", msgBuf);
        }
        return ret;
    }

#endif

#if OPEN_VPRINT
    int LoggerHook::hookLogVPrint(int prio, const char *tag, const char *fmt, va_list ap) {
        char msgBuf[1024];
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, ap);
        MGLog log{};
        log.tid = my_tid();
        log.tag = tag ? tag : "unknown";
        strncpy(log.msg, msgBuf, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG,
                           LOG_VPRINT,
                           "hook_log_vprint: prio=%d, tag=%s ,tid=%lld, msg=%s ,ts=%lld",
                           prio, log.tag, log.tid, log.msg, log.ts);
        }
        int ret = 0;
        if (orig_log_vprint) {
            ret = orig_log_vprint(prio, tag ? tag : "NULL", "%s", msgBuf);
        }
        return ret;
    }
#endif

#if OPEN_BUF_WRITE

    int LoggerHook::hookLogBufWrite(int bufID, int prio, const char *tag, const char *text) {
        MGLog log{};
        log.tid = my_tid();
        log.tag = tag ? tag : "unknown";
        strncpy(log.msg, text ? text : "", sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();

        static thread_local bool reentry = false;
        if (!reentry && orig_log_buf_write) {
            reentry = true;  // 防止调试日志再进这里
            char dbg[512];
            snprintf(dbg, sizeof(dbg),
                     "hookLogBufWrite: bufID=%d prio=%d tag=%s tid=%lld text=%s",
                     bufID, prio, log.tag,
                     (long long) my_tid(), log.msg);
            // 用 MAIN 缓冲区；0 == LOG_ID_MAIN
            orig_log_buf_write(0 /*LOG_ID_MAIN*/,
                               ANDROID_LOG_DEBUG, LOG_BUF_WRITE, dbg);
            reentry = false;
        }

        int ret = 0;
        if (orig_log_buf_write) {
            ret = orig_log_buf_write(bufID, prio, tag, text);
        }
        return ret;
    }

#endif

    void LoggerHook::writeLog(MGLog *log, int tag) {
        if (!m_loggerQueue) {
            ALOGE("LoggerQueue is not initialized");
        }
        enqueue(log, tag);
    }

    void LoggerHook::enqueue(MGLog *log, int tag) {
        m_loggerQueue->enqueue(log, tag);
    }

    int LoggerHook::dequeue(MGLog *log) {
        int ret = m_loggerQueue->dequeue(log);
        return ret;
    }

}
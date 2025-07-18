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

using namespace MGLogger;

LoggerHook *LoggerHook::s_instance = nullptr;

LoggerHook::LoggerHook() : m_loggerQueue(std::make_shared<LoggerQueue>(1000)) {
    ALOGD("LoggerHook::LoggerHook - initialized");
    s_instance = this;
}

LoggerHook::~LoggerHook() {
    ALOGD("LoggerHook::~LoggerHook - shutting down");
    if (m_loggerQueue) {
        m_loggerQueue->abort();
        m_loggerQueue->clear();
    }
    s_instance = nullptr;
}

void LoggerHook::init() {
    ALOGD("LoggerHook::init - registering log hooks");
    if (orig_log_print || orig_log_write || orig_log_buf_write || orig_log_vprint) {
        return;
    }

#if OPEN_WRITE
    if (xhook_register(".*\\.so$", LOG_WRITE, (void*)hookLogWrite, (void**)&orig_log_write) != 0) {
            ALOGE("LoggerHook::init - Failed to hook __android_log_write");
        } else {
            ALOGD("LoggerHook::init - Hooked __android_log_write");
        }
#endif

#if OPEN_PRINT
    if (xhook_register(".*\\.so$", LOG_PRINT, (void*)hookLogPrint, (void**)&orig_log_print) != 0) {
            ALOGE("LoggerHook::init - Failed to hook __android_log_print");
        } else {
            ALOGD("LoggerHook::init - Hooked __android_log_print");
        }
#endif
#if OPEN_VPRINT
    if (xhook_register(".*\\.so$", LOG_VPRINT, (void*)hookLogVPrint, (void**)&orig_log_vprint) != 0) {
            ALOGE("LoggerHook::init - Failed to hook __android_log_vprint");
        } else {
            ALOGD("LoggerHook::init - Hooked __android_log_vprint");
        }
#endif

#if OPEN_BUF_WRITE
    if (xhook_register(".*\\.so$", LOG_BUF_WRITE, (void *) hookLogBufWrite,
                       (void **) &orig_log_buf_write) != 0) {
        ALOGE("LoggerHook::init - Failed to hook __android_log_buf_write");
    } else {
        ALOGD("LoggerHook::init - Hooked __android_log_buf_write");
    }
#endif

#if OPEN_ASSERT
    if (xhook_register(".*\\.so$", LOG_ASSERT, (void*)hookLogAssert, (void**)&orig_log_assert) != 0) {
            ALOGE("LoggerHook::init - Failed to hook __android_log_assert");
        } else {
            ALOGD("LoggerHook::init - Hooked __android_log_assert");
        }
#endif
    // 应用所有挂钩
    xhook_refresh(0);
    ALOGD("LoggerHook::init - log hooks installed");
}


void LoggerHook::enqueue(MGLog *log, int sourceType) {
    if (!m_loggerQueue) {
        ALOGE("LoggerHook::enqueue - LoggerQueue not initialized");
        return;
    }
    m_loggerQueue->enqueue(log, static_cast<LogSourceType>(sourceType));
}

void LoggerHook::writeLog(MGLog *log, int sourceType) {
    if (!m_loggerQueue) {
        ALOGE("LoggerHook::writeLog - LoggerQueue not initialized");
        return;
    }
    // 将日志封装入队列
    enqueue(log, sourceType);
}

int LoggerHook::dequeue(MGLog *log) {
    if (!m_loggerQueue) {
        ALOGE("LoggerHook::dequeue - LoggerQueue not initialized");
        return -1;
    }
    return m_loggerQueue->dequeue(log);
}

void LoggerHook::stop() {
    // 中止日志队列，通知消费者线程退出
    if (m_loggerQueue) {
        m_loggerQueue->abort();
    }
}

#if OPEN_WRITE
int LoggerHook::hookLogWrite(int prio, const char *tag, const char *buf) {
        if (buf) {
            MGLog log{};
            log.tid = my_tid();
            strncpy(log.tag, tag ? tag : "unknown", sizeof(log.tag) - 1);
            log.tag[sizeof(log.tag) - 1] = '\0';
            strncpy(log.msg, buf, sizeof(log.msg) - 1);
            log.msg[sizeof(log.msg) - 1] = '\0';
            log.ts = getCurrentTimeMillis();
            // 将日志加入队列（过滤掉内部日志，以免递归）
            if (s_instance && tag && strcmp(tag, MGLOGGER_LOG_TAG) != 0) {
                s_instance->writeLog(&log, LOG_SRC_WRITE);
            }
        }
#if DEBUG_LOG
        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG, "MG_LOGGER",
                           "hookLogWrite: prio=%d, tag=%s, tid=%lld",
                           prio, tag ? tag : "NULL", (long long)my_tid());
        }
#endif
        int result = 0;
        if (orig_log_write) {
            result = orig_log_write(prio, tag ? tag : "NULL", buf ? buf : "");
        }
        return result;
    }
#endif

#if OPEN_PRINT

int LoggerHook::hookLogPrint(int prio, const char *tag, const char *fmt, ...) {
        char msgBuf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
        va_end(args);
        MGLog log{};
        log.tid = my_tid();
        strncpy(log.tag, tag ? tag : "unknown", sizeof(log.tag) - 1);
        log.tag[sizeof(log.tag) - 1] = '\0';
        strncpy(log.msg, msgBuf, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
        if (s_instance && tag && strcmp(tag, MGLOGGER_LOG_TAG) != 0) {
            s_instance->writeLog(&log, LOG_SRC_PRINT);
        }
#if DEBUG_LOG
        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG, "MG_LOGGER",
                           "hookLogPrint: prio=%d, tag=%s, tid=%lld",
                           prio, tag ? tag : "NULL", (long long)my_tid());
        }
#endif
        int result = 0;
        if (orig_log_print) {
            // 调用原始函数输出原始日志
            result = orig_log_print(prio, tag ? tag : "NULL", "%s", msgBuf);
        }
        return result;
    }

#endif

#if OPEN_VPRINT
int LoggerHook::hookLogVPrint(int prio, const char *tag, const char *fmt, va_list ap) {
        char msgBuf[1024];
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, ap);
        MGLog log{};
        log.tid = my_tid();
        strncpy(log.tag, tag ? tag : "unknown", sizeof(log.tag) - 1);
        log.tag[sizeof(log.tag) - 1] = '\0';
        strncpy(log.msg, msgBuf, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
        if (s_instance && tag && strcmp(tag, MGLOGGER_LOG_TAG) != 0) {
            s_instance->writeLog(&log, LOG_SRC_VPRINT);
        }
#if DEBUG_LOG
        if (orig_log_print) {
            orig_log_print(ANDROID_LOG_DEBUG, "MG_LOGGER",
                           "hookLogVPrint: prio=%d, tag=%s, tid=%lld",
                           prio, tag ? tag : "NULL", (long long)my_tid());
        }
#endif
        int result = 0;
        if (orig_log_vprint) {
            // 调用原始函数输出原始日志
            result = orig_log_vprint(prio, tag ? tag : "NULL", "%s", msgBuf);
        }
        return result;
    }
#endif

#if OPEN_BUF_WRITE

int LoggerHook::hookLogBufWrite(int bufID, int prio, const char *tag, const char *text) {
    if (text) {
        MGLog log{};
        log.tid = my_tid();
        strncpy(log.tag, tag ? tag : "unknown", sizeof(log.tag) - 1);
        log.tag[sizeof(log.tag) - 1] = '\0';
        strncpy(log.msg, text, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
        // 将日志加入队列（过滤内部的 MG_LOGGER 日志）
        if (s_instance && tag && strcmp(tag, MGLOGGER_LOG_TAG) != 0) {
            s_instance->writeLog(&log, LOG_SRC_BUF_WRITE);
        }
#if DEBUG_LOG
        static thread_local bool reentry = false;
        if (!reentry && orig_log_buf_write) {
            reentry = true;  // 防止调试日志再进这里
            char dbg[512];
            snprintf(dbg, sizeof(dbg),
                     "tag=%s tid=%lld log=%s",
                     log.tag, (long long) my_tid(), log.msg);
            // 用 MAIN 缓冲区；0 == LOG_ID_MAIN
            orig_log_buf_write(0 /*LOG_ID_MAIN*/,
                               ANDROID_LOG_DEBUG, LOG_BUF_WRITE, dbg);
            reentry = false;
        }
#endif
    }

    int ret = 0;
    if (orig_log_buf_write) {
        ret = orig_log_buf_write(bufID, prio, tag, text);
    }
    return ret;
}

#endif

#if OPEN_ASSERT
void LoggerHook::hookLogAssert(const char *cond, const char *tag, const char *fmt, ...) {
        // 格式化断言日志消息
        char msgBuf[1024];
        va_list args, args_copy;
        va_start(args, fmt);
        va_copy(args_copy, args);
        vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
        va_end(args);
        char fullMsg[1280];
        snprintf(fullMsg, sizeof(fullMsg), "[ASSERT:%s] %s",
                 cond ? cond : "null", msgBuf);
        MGLog log{};
        log.tid = my_tid();
        strncpy(log.tag, tag ? tag : "unknown", sizeof(log.tag) - 1);
        log.tag[sizeof(log.tag) - 1] = '\0';
        strncpy(log.msg, fullMsg, sizeof(log.msg) - 1);
        log.msg[sizeof(log.msg) - 1] = '\0';
        log.ts = getCurrentTimeMillis();
        if (s_instance && tag && strcmp(tag, MGLOGGER_LOG_TAG) != 0) {
            s_instance->writeLog(&log, LOG_SRC_ASSERT);
        }
        if (orig_log_assert) {
            // 调用原始断言函数（可能会终止进程）
            orig_log_assert(cond, tag ? tag : "unknown", fmt, args_copy);
        }
        va_end(args_copy);
    }
#endif
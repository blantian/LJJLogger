/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 14:50
 */

#pragma once

#include "mglogger.h"

namespace MGLogger {

    MGLogger::MGLogger() {
        m_mutex = SDL_CreateMutex();
        if (!m_mutex) {
            ALOGE("MGLogger::MGLogger - Failed to create mutex");
        }
        m_cond = SDL_CreateCond();
        if (!m_cond) {
            ALOGE("MGLogger::MGLogger - Failed to create cond");
        }
        running = false;
    }

    MGLogger::~MGLogger() {
        stop();
    }

    int MGLogger::init(const char *cache_path,
                       const char *dir_path,
                       int log_cache_s,
                       int max_file,
                       const char *key16,
                       const char *iv16) {
        // 初始化 CLogan 环境，根据配置决定是否使用钩子线程
        if (log_cache_s == 1) {
            // 使用 Hook 方式拦截日志（在当前进程）
            loggerHook = std::make_shared<LoggerHook>();
            // 创建并启动后台日志消费线程
            m_worker_tid = createEnqueueTh();
            if (!m_worker_tid) {
                ALOGE("MGLogger::init - Failed to create logger thread");
                return MG_ERROR;
            }
            // 安装日志钩子
            loggerHook->init();
        } else {
            // TODO: 使用独立进程记录日志的实现
        }
        // 初始化 CLogan 日志库
        SDL_LockMutex(m_mutex);
        int result = clogan_init(cache_path ? cache_path : "",
                                 dir_path ? dir_path : "",
                                 max_file,
                                 key16 ? key16 : "",
                                 iv16 ? iv16 : "");
        SDL_UnlockMutex(m_mutex);
        return result;
    }


    SDL_Thread *MGLogger::createEnqueueTh() {
        running = true;
        return SDL_CreateThreadEx(&_m_worker_thread, &threadFunc, this, "logan_worker");
    }

    int MGLogger::threadFunc(void *arg) {
        auto self = static_cast<MGLogger*>(arg);
        ALOGI("MGLogger::threadFunc - Logger thread started");
        int ret = self->run();
        ALOGI("MGLogger::threadFunc - Logger thread exiting (ret=%d)", ret);
        return ret;
    }


    int MGLogger::run() {
        ALOGD("MGLogger::run - Entering log consumption loop");
        MGLog logEntry;
        while (true) {
            // 阻塞等待获取一条日志
            int sourceType = loggerHook->dequeue(&logEntry);
            if (sourceType < 0) {
                // 返回 -1 表示队列已中止且无日志，退出循环
                break;
            }
            // 写入获取的日志到持久化存储（CLogan）
            int writeRet = write(&logEntry);
            if (writeRet < 0) {
                ALOGE("MGLogger::run - Failed to write log (tid=%lld, tag=%s, writeRet=%d)",
                      logEntry.tid, logEntry.tag, writeRet);
                // 写入失败，仅记录错误，不中断循环，继续处理下一条
            }
        }
        // 所有剩余日志处理完毕后，刷新缓存数据到文件
        flush();
        running = false;
        return MG_OK;
    }

    int MGLogger::open(const char *file_name) {
        SDL_LockMutex(m_mutex);
        int code = clogan_open(file_name ? file_name : "");
        SDL_UnlockMutex(m_mutex);
        return code;
    }


    int MGLogger::write(int flag, const char *log, long long local_time,
                        const char *thread_name, long long thread_id, int is_main) {
        std::string logStr(log ? log : "");
        std::string threadNameStr(thread_name ? thread_name : "");
        int code = clogan_write(flag,
                                const_cast<char *>(logStr.c_str()),
                                local_time,
                                const_cast<char *>(threadNameStr.c_str()),
                                thread_id,
                                is_main);
        return code;
    }

    int MGLogger::write(MGLog *log) {
        if (!log) {
            ALOGE("MGLogger::write - Invalid log (null)");
            return MG_ERROR;
        }
        // 确保日志字段合法
        if (log->tag[0] == '\0') {
            strncpy(log->tag, "default", sizeof(log->tag) - 1);
            log->tag[sizeof(log->tag) - 1] = '\0';
        }
        if (log->ts <= 0) {
            log->ts = static_cast<long long>(time(nullptr)) * 1000LL;
        }
        // 判断日志是否来自主线程
        int is_main = (log->tid == getpid()) ? 1 : 0;
        // 写入日志到 CLogan（日志类型 flag=0 表示普通日志）
        SDL_LockMutex(m_mutex);
        int code = clogan_write(0,
                                log->msg,
                                log->ts,
                                const_cast<char *>(log->tag),
                                log->tid,
                                is_main);
        SDL_UnlockMutex(m_mutex);
        return code;
    }

    int MGLogger::flush() {
        SDL_LockMutex(m_mutex);
        int code = clogan_flush();
        SDL_UnlockMutex(m_mutex);
        return code;
    }

    void MGLogger::debug(int debug) {
        SDL_LockMutex(m_mutex);
        clogan_debug(debug);
        SDL_UnlockMutex(m_mutex);
    }

    void MGLogger::stop() {
        // 发出停止信号，让日志线程优雅退出
        if (loggerHook) {
            loggerHook->stop();
        }
        if (m_worker_tid) {
            SDL_WaitThread(m_worker_tid, nullptr);
            m_worker_tid = nullptr;
        }
        // 销毁同步资源
        if (m_mutex) { SDL_DestroyMutex(m_mutex); m_mutex = nullptr; }
        if (m_cond)  { SDL_DestroyCond(m_cond);  m_cond  = nullptr; }
        // 释放 LoggerHook（此时其析构会清理队列）
        loggerHook.reset();
    }

}
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 14:50
 */

#pragma once

#include "mglogger.h"

namespace MGLogger {

    MGLogger::MGLogger(){
        m_mutex = SDL_CreateMutex();
        if (!m_mutex) {
            ALOGE("Failed to create logger mutex");
        }
        m_cond = SDL_CreateCond();
        if (!m_cond) {
            ALOGE("Failed to create logger cond");
        }
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
        std::string cachePathStr(cache_path ? cache_path : "");
        std::string dirPathStr(dir_path ? dir_path : "");
        std::string keyStr(key16 ? key16 : "");
        std::string ivStr(iv16 ? iv16 : "");

        if (log_cache_s == 1) {
            loggerHook = std::make_shared<LoggerHook>();
            m_worker_tid = createEnqueueTh();
            if (!m_worker_tid){
                ALOGE("MGLogger::init: Failed to create worker thread");
                return -1;
            }
            loggerHook->init();
        } else {
//            // fork 进程
        }
        SDL_LockMutex(m_mutex);
        int code = clogan_init(cachePathStr.c_str(), dirPathStr.c_str(), max_file,
                               keyStr.c_str(), ivStr.c_str());
        SDL_UnlockMutex(m_mutex);
        return code;
    }

    SDL_Thread *MGLogger::createEnqueueTh() {
        running = true;
        return SDL_CreateThreadEx(&_m_worker_thread, &threadFunc, this, "logan_worker");
    }

    int MGLogger::threadFunc(void *arg) {
        auto self = (MGLogger *) arg;
        ALOGI("MGLogger::threadFunc: starting logger thread");
        int ret = self->run();
        ALOGI("MGLogger::threadFunc: logger thread finished with ret=%d", ret);
        return ret;
    }

    int MGLogger::run() {
        int ret = 0;
        ALOGD("MGLogger::run: starting logger loop");
        while (running) {
            if (!loggerHook) {
                ALOGD("MGLogger::run: loggerHook is null, waiting for initialization");
                continue;
            }
//            MGLog *log = nullptr;
//            ret = loggerHook->dequeue(log);
//            if (ret < 0) {
//                ALOGE("MGLogger::run: dequeue log failed, ret=%d", ret);
//            }
//            ret = write(log);
//            if (ret < 0) {
//                ALOGE("MGLogger::run: write log failed, ret=%d", ret);
//            }
        }
        return ret;
    }

    int MGLogger::open(const char *file_name) {
        std::string fileNameStr(file_name ? file_name : "");
        SDL_LockMutex(m_mutex);
        int code = clogan_open(fileNameStr.c_str());
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
        ALOGD("MGLogger::write: log=%p, msg=%s, tag=%s, ts=%lld, tid=%lld",
              log, log ? log->msg : "null", log ? (log->tag ? log->tag : "null") : "null",
              log ? log->ts : 0, log ? log->tid : 0);
        if (!log) {
            ALOGE("MGLogger::write: log is null");
            return -1;
        }
        SDL_LockMutex(m_mutex);
        char *log_msg = log->msg;
        if (log_msg[0] == '\0') {
            ALOGE("MGLogger::write: log message is empty");
            SDL_UnlockMutex(m_mutex);
            return -1;
        }
        if (log->tag[0] == '\0') {
            strncpy(log->tag, "default", sizeof(log->tag) - 1);
        }
        if (log->ts <= 0) {
            log->ts = static_cast<long long>(time(nullptr)) * 1000LL;
        }

        const char *thread_name = "unknown";
        int code = clogan_write(0, log_msg, log->ts, const_cast<char *>(thread_name), log->tid, 0);
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

        if (m_worker_tid) {
            SDL_WaitThread(m_worker_tid, nullptr);
            m_worker_tid = nullptr;
        }
        if (m_mutex) {
            SDL_DestroyMutex(m_mutex);
            m_mutex = nullptr;
        }
        if (m_cond) {
            SDL_DestroyCond(m_cond);
            m_cond = nullptr;
        }
    }

}
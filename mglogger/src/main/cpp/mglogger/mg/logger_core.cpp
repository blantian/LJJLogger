/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 14:50
 */

#pragma once

#include "logger_core.h"

#include <utility>

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
        alive = false;
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
        ALOGD("MGLogger::init - Initializing MGLogger with cache_path=%s, dir_path=%s, max_file=%d key16=%s, iv16=%s in Android API level %d",
              cache_path ? cache_path : "null",
              dir_path ? dir_path : "null",
              max_file, key16 ? key16 : "null", iv16 ? iv16 : "null", SDL_Android_GetApiLevel());

        if (m_mutex == nullptr || m_cond == nullptr) {
            ALOGE("MGLogger::init - Mutex or Cond not initialized");
            return MG_ERROR;
        }

        _listener = std::make_shared<MessageQueue>();
        if (!_listener) {
            ALOGE("MGLogger::init - Failed to create MessageQueue");
            return MG_ERROR;
        }
        _listener->start();
        // 创建消息处理线程
        m_message_tid = createMessageTh();
        if (!m_message_tid) {
            ALOGE("MGLogger::init - Failed to create message thread");
            return MG_ERROR;
        }
        SDL_LockMutex(m_mutex);
        int result = clogan_init(cache_path ? cache_path : "",
                                 dir_path ? dir_path : "",
                                 max_file,
                                 key16 ? key16 : "",
                                 iv16 ? iv16 : "");
        SDL_UnlockMutex(m_mutex);
        if (result == CLOGAN_INIT_SUCCESS_MMAP || result == CLOGAN_INIT_SUCCESS_MEMORY) {
            int code = 0;
            ALOGD("MGLogger::init - CLogan initialized successfully (code=%d)", result);
            code = clogan_open(MG_LOGGER_LOG);
            ALOGD("MGLogger::init - CLogan opened logan.bin (code=%d)", code);
            clogan_flush();
            code = clogan_write(0, "MGLogger initialized", 0, "MGLogger", getpid(), 1);
            if (code == CLOGAN_WRITE_SUCCESS) {
                ALOGE("MGLogger::init - Write initial log success (code=%d)", code);
            }
            mLogger = ILogger::CreateLogger(log_cache_s);
            m_worker_tid = createEnqueueTh();
            if (!m_worker_tid) {
                ALOGE("MGLogger::init - Failed to create logger thread");
                return MG_ERROR;
            }
            // 初始化日志钩子
            result = mLogger->init();
        } else {
            ALOGE("MGLogger::init - CLogan initialization failed (code=%d)", result);
            return result;
        }
        return result;
    }


    void MGLogger::setBlackList(const std::list<std::string> &blackList) {
        SDL_LockMutex(m_mutex);
        if (mLogger) {
            mLogger->setBlackList(blackList);
        } else {
            ALOGE("MGLogger::setBlackList - Logger not initialized, cannot set blacklist");
        }
        SDL_UnlockMutex(m_mutex);
    }


    SDL_Thread *MGLogger::createEnqueueTh() {
        running = true;
        return SDL_CreateThreadEx(&_m_worker_thread, &threadFunc, this, "logan_worker");
    }

    SDL_Thread *MGLogger::createMessageTh() {
        alive = true;
        return SDL_CreateThreadEx(&_m_message_thread, &messageThreadFunc, this,
                                  "logan_message_worker");
    }

    int MGLogger::threadFunc(void *arg) {
        auto self = static_cast<MGLogger *>(arg);
        ALOGI("MGLogger::threadFunc - Logger thread started");
        int ret = self->run();
        ALOGI("MGLogger::threadFunc - Logger thread exiting (ret=%d)", ret);
        return ret;
    }


    int MGLogger::messageThreadFunc(void *arg) {
        auto self = static_cast<MGLogger *>(arg);
        ALOGI("MGLogger::messageThreadFunc - Message thread started");
        int ret = self->runMessageThread();
        ALOGI("MGLogger::messageThreadFunc - Message thread exiting (ret=%d)", ret);
        return ret;
    }


    int MGLogger::run() {
        ALOGD("MGLogger::run - Entering log consumption loop");
        MGLog logEntry;
        mBatchBuf.reserve(BATCH_SIZE);          // 预分配一次即可
        while (running) {
            // 阻塞等待获取一条日志
            int sourceType = mLogger->dequeue(&logEntry);
            if (sourceType < 0) {
                // 返回 -1 表示队列已中止且无日志，退出循环
                break;
            }

            mBatchBuf.emplace_back(logEntry);   // 放进批量容器
            uint64_t now = nowMs(); // 获取当前时间戳（毫秒）
            bool sizeLimit   = mBatchBuf.size() >= BATCH_SIZE;
            bool timeExpired = now - mLastFlushTs >= FLUSH_INTERVAL_MS;
            ALOGD("MGLogger::run - Log entry received (tid=%lld, tag=%s, sizeLimit=%d, timeExpired=%d)",
                  logEntry.tid, logEntry.tag, sizeLimit, timeExpired);
            // 写入获取的日志到持久化存储
            if (sizeLimit || timeExpired) {
                SDL_LockMutex(m_mutex);
                for (auto &item : mBatchBuf) {
                    int code = write(&item);
                    if (code == CLOGAN_WRITE_SUCCESS) {
                        ALOGI("MGLogger::run - Log written successfully (tid=%lld, tag=%s, writeRet=%d)",
                              logEntry.tid, logEntry.tag, code);
                    } else {
                        ALOGE("MGLogger::run - Failed to write log (tid=%lld, tag=%s, writeRet=%d)",
                              logEntry.tid, logEntry.tag, code);
                        // todo 处理写入失败的情况,暂时忽略
                    }
                }
                mBatchBuf.clear();
                mLastFlushTs = now;
                SDL_UnlockMutex(m_mutex);
            }
        }
        SDL_LockMutex(m_mutex);
        //处理残留日志
        for (auto &item : mBatchBuf) {
            write(&item);
        }
        mBatchBuf.clear();
        mLastFlushTs = 0; // 更新最后刷新时间戳
        SDL_UnlockMutex(m_mutex);
        // 所有剩余日志处理完毕后，刷新缓存数据到文件
        flush();
        running = false;
        ALOGD("MGLogger::run - Logger thread exiting (running=%d)", running);
        return MG_OK;
    }

    void MGLogger::SetOnEventListener(std::shared_ptr<OnEventListener> listener) {
        SDL_LockMutex(m_mutex);
        _eventListener = std::move(listener);
        SDL_UnlockMutex(m_mutex);
    }

    int MGLogger::runMessageThread() {
        ALOGD("MGLogger::runMessageThread - Entering message consumption loop");
        while (alive) {
            if (m_message_tid == nullptr) {
                ALOGW("MGLogger::runMessageThread - Message thread is null, exiting loop");
                break;
            }

            if (!_listener) {
                ALOGW("MGLogger::runMessageThread - MessageQueue is null, exiting loop");
                break;
            }
            // 阻塞等待获取一条消息
            std::shared_ptr<MGMessage> msg = _listener->getMessage();
            if (msg == nullptr) {
                continue; // 没有消息，继续等待
            }
            // 处理获取的消息
            handleMessage(msg);
        }
        // 所有剩余日志处理完毕后，刷新缓存数据到文件
        alive = false;
        m_message_tid = nullptr;
        return MG_OK;
    }

    /**
     * 处理消息
     * @param msg
     */
    void MGLogger::handleMessage(const std::shared_ptr<MGMessage> &msg) {
        switch (msg->what) {

        }
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
        int code = clogan_write(0,
                                log->msg,
                                log->ts,
                                const_cast<char *>(log->tag),
                                log->tid,
                                is_main);
        return code;
    }

    int MGLogger::flush() {
        SDL_LockMutex(m_mutex);
        int code = clogan_flush();
        if (code == CLOGAN_FLUSH_SUCCESS) {
            ALOGI("MGLogger::flush - CLogan flush successful");
        } else {
            ALOGE("MGLogger::flush - CLogan flush failed (code=%d)", code);
        }
        SDL_UnlockMutex(m_mutex);
        return code;
    }

    void MGLogger::debug(int debug) {
        SDL_LockMutex(m_mutex);
        clogan_debug(debug);
        SDL_UnlockMutex(m_mutex);
    }

    void MGLogger::stop() {
        ALOGI("MGLogger::stop - Stopping logger");
        flush();
        if (mLogger) {
            mLogger->stop();
        }
        if (m_worker_tid) {
            SDL_WaitThread(m_worker_tid, nullptr);
            m_worker_tid = nullptr;
        }

        if (m_message_tid) {
            SDL_WaitThread(m_message_tid, nullptr);
            m_message_tid = nullptr;
        }

        // 销毁同步资源
        if (m_mutex) {
            SDL_DestroyMutex(m_mutex);
            m_mutex = nullptr;
        }
        if (m_cond) {
            SDL_DestroyCond(m_cond);
            m_cond = nullptr;
        }
        // 释放 LoggerHook
        mLogger.reset();
    }

}
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 14:50
 */

#include "logger_core.h"

#include <utility>
#include <algorithm>
#include <cstdio>

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
                       int max_sdcard_size,
                       const char *key16,
                       const char *iv16) {

        int result = MG_OK;
        // 初始化 CLogan 环境，根据配置决定是否使用钩子线程
        ALOGD("MGLogger::init - Initializing MGLogger with cache_path=%s, dir_path=%s, max_file=%d max_sdcard_size=%d key16=%s, iv16=%s in Android API level %d",
              cache_path ? cache_path : "null",
              dir_path ? dir_path : "null",
              max_file, max_sdcard_size, key16 ? key16 : "null", iv16 ? iv16 : "null",
              SDL_Android_GetApiLevel());

        if (m_mutex == nullptr || m_cond == nullptr) {
            ALOGE("MGLogger::init - Mutex or Cond not initialized");
            return MG_ERROR;
        }

        SDL_LockMutex(m_mutex);
        clogan_debug(0);
        result = clogan_init(cache_path ? cache_path : "",
                             dir_path ? dir_path : "",
                             max_file,
                             key16 ? key16 : "",
                             iv16 ? iv16 : "");
        if (result == CLOGAN_INIT_SUCCESS_MMAP || result == CLOGAN_INIT_SUCCESS_MEMORY) {
            int code = 0;
            if (result == CLOGAN_INIT_SUCCESS_MMAP) {
                ALOGD("MGLogger::init - CLogan initialized with mmap (code=%d)", result);
            } else {
                ALOGD("MGLogger::init - CLogan initialized with memory (code=%d)", result);
            }
            // 防止重启无线创建缓存文件，默认两个小时无更新不创建
            std::string reuseFile;
            if (dir_path && *dir_path) {
                auto infos = utils::LoggerUtils::collectFileInfo(dir_path);
                uint64_t now = utils::LoggerUtils::nowMs();
                uint64_t latestTs = 0;
                for (const auto &kv : infos) {
                    uint64_t ts = 0;
                    try {
                        ts = utils::LoggerUtils::parseTsFromFileName(kv.first);
                    } catch (...) {
                        continue;
                    }
                    if (now > ts && now - ts <= TWO_HOURS_MS &&
                        kv.second < max_file && ts > latestTs) {
                        latestTs = ts;
                        reuseFile = kv.first;
                    }
                }
            }

            const char *fileName;
            if (!reuseFile.empty()) {
                fileName = reuseFile.c_str();
                ALOGI("MGLogger::init - Reuse recent log file %s", fileName);
            } else {
                uint64_t now = utils::LoggerUtils::nowMs();
                fileName = utils::LoggerUtils::toCString(now);
                ALOGI("MGLogger::init - No recent log file found, using new file %s", fileName);
            }
            code = clogan_open(fileName);
            ALOGD("MGLogger::init - Logger CLogan opened file (fileName=%s,code=%d)",fileName, code);
            code = clogan_flush();
            ALOGD("MGLogger::init - Logger CLogan flushed logan (code=%d)", code);
            char log_buf[256];
            snprintf(log_buf, sizeof(log_buf), "MGLogger initialized with file %s", fileName);
            // 获取当前线程名
//            const char *thread_name = SDL_ThreadGetName(SDL_ThreadID());
            code = clogan_write(0, log_buf, 0, "MGLogger", getpid(), 1);
            if (code == CLOGAN_WRITE_SUCCESS) {
                ALOGI("MGLogger::init - Write initial log success (code=%d)", code);
            }
            SDL_UnlockMutex(m_mutex);
            // 创建日志处理器
            mLogger = BaseLogger::CreateLogger(log_cache_s);
            if (!mLogger) {
                ALOGE("MGLogger::init - Failed to create ILogger instance");
                return MG_LOGGER_CREATE_FAILED;
            }
            mMaxSingleFileSize = max_file; // 设置单个文件最大大小
            mMaxSDCardFileSize = max_sdcard_size + LOG_EXTERNAL_SIZE; // 设置sdCard最大文件大小
            mCacheFilePath = dir_path ? dir_path : ""; // 设置缓存文件路径
            // 初始化日志钩子/logcat 进程
            result = mLogger->init();
            if (result != MG_OK) {
                ALOGE("MGLogger::init - ILogger initialization failed (code=%d)", result);
                return result;
            }

            result = mLogger->start();
            if (result != MG_OK) {
                ALOGE("MGLogger::init - ILogger start failed (code=%d)", result);
                return result;
            }

            // 创建日志处理线程
            m_worker_tid = createEnqueueTh();
            if (!m_worker_tid) {
                ALOGE("MGLogger::init - Failed to create logger thread");
                return MG_LOGGER_CREATE_WORKER_THREAD_FAILED;
            }

            // 创建消息处理线程
            m_message_tid = createMessageTh();
            if (!m_message_tid) {
                ALOGE("MGLogger::init - Failed to create message thread");
                return MG_LOGGER_CREATE_MESSAGE_THREAD_FAILED;
            }
        } else {
            ALOGE("MGLogger::init - CLogan initialization failed (code=%d)", result);
            return result;
        }
        return result;
    }


    void MGLogger::setBlackList(const std::list<std::string> &blackList) {
        ALOGD("MGLogger::setBlackList - Setting blacklist with %zu items", blackList.size());
        SDL_LockMutex(m_mutex);
        mBlackList = blackList;
        if (mLogger) {
            mLogger->setBlackList(mBlackList);
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

    void MGLogger::stopThreads() {
        ALOGD("MGLogger::stopThreads - Stopping logger threads");
        running = false;
        alive = false;
        if (mLogger) {
            mLogger->stop();
        }

        if (m_worker_tid) {
            if (!pthread_equal(m_worker_tid->id, pthread_self())) {
                ALOGD("MGLogger::stopThreads - Waiting for worker thread to finish");
                SDL_WaitThread(m_worker_tid, nullptr);
            } else {
                ALOGW("MGLogger::stopThreads - Called from worker thread, skipping wait");
            }
            m_worker_tid = nullptr;
            ALOGD("MGLogger::stopThreads - Worker thread stopped");
        }

        if (m_message_tid) {
            if (!pthread_equal(m_message_tid->id, pthread_self())) {
                ALOGD("MGLogger::stopThreads - Waiting for message thread to finish");
                SDL_WaitThread(m_message_tid, nullptr);
            } else {
                ALOGW("MGLogger::stopThreads - Called from message thread, skipping wait");
            }
            m_message_tid = nullptr;
            ALOGD("MGLogger::stopThreads - Message thread stopped");
        }
    }

    bool MGLogger::startThreads() {
        ALOGD("MGLogger::startThreads - Starting logger threads");
        m_worker_tid = createEnqueueTh();
        if (!m_worker_tid) {
            ALOGE("MGLogger::startThreads - Failed to create worker thread");
            return false;
        }
        m_message_tid = createMessageTh();
        if (!m_message_tid) {
            ALOGE("MGLogger::startThreads - Failed to create message thread");
            SDL_WaitThread(m_worker_tid, nullptr);
            m_worker_tid = nullptr;
            return false;
        }
        return true;
    }

    /**
     * 日志处理线程函数
     * @param arg
     * @return
     */
    int MGLogger::threadFunc(void *arg) {
        auto self = static_cast<MGLogger *>(arg);
        ALOGI("MGLogger::threadFunc - Logger thread started");
        int ret = self->run();
        ALOGI("MGLogger::threadFunc - Logger thread exited (ret=%d)", ret);
        return ret;
    }


    /**
     * 消息处理线程函数
     * @param arg
     * @return
     */
    int MGLogger::messageThreadFunc(void *arg) {
        auto self = static_cast<MGLogger *>(arg);
        ALOGI("MGLogger::messageThreadFunc - Message thread started");
        int ret = self->runMessageThread();
        ALOGI("MGLogger::messageThreadFunc - Message thread exiting (ret=%d)", ret);
        return ret;
    }


    /**
     * 日志处理线程函数
     */
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

            //todo 先不开，会出现频繁flush，导致日志丢失 （最大缓存2M，mmap缓存150k，当文件达到2.85M以上的时候，mmap如果是150k会出现丢失问题）
//            if (logEntry.level == LEVEL_ERROR) {
//                write(&logEntry); // 直接写入错误日志
//                clogan_flush(); // 如果是错误日志，立即刷新
//                continue;
//            }

            mBatchBuf.emplace_back(logEntry);   // 放进批量容器
            uint64_t now = utils::LoggerUtils::nowMs(); // 获取当前时间戳（毫秒）
            bool sizeLimit = mBatchBuf.size() >= BATCH_SIZE;
            bool timeExpired = now - mLastFlushTs >= FLUSH_INTERVAL_MS;
            ALOGD("MGLogger::run - Log entry received (tid=%lld, tag=%s, sizeLimit=%d, timeExpired=%d)",
                  logEntry.tid, logEntry.tag, sizeLimit, timeExpired);
            // 写入获取的日志到持久化存储
            if (sizeLimit || timeExpired) {
                SDL_LockMutex(m_mutex);
                for (auto &item: mBatchBuf) {
                    int code = write(&item);
                    switch (code) {
                        case CLOGAN_WRITE_SUCCESS:
                            break;
                        case CLOAGN_WRITE_FAIL_MAXFILE:
                            ALOGE("MGLogger::run - Failed to write log (tid=%lld, tag=%s, code=%d)",
                                  item.tid, item.tag, code);
                            if (reWrite(&item) == CLOGAN_WRITE_SUCCESS) {
                                ALOGD("MGLogger::run - Retried log write after max file limit (tid=%lld, tag=%s)",
                                      item.tid, item.tag);
                            } else {
                                ALOGE("MGLogger::run - Retried log write failed (tid=%lld, tag=%s)",
                                      item.tid, item.tag);
                            }
                            break;
                        case CLOGAN_WRITE_FAIL_HEADER:
                            ALOGE("MGLogger::run - Failed to write log header (tid=%lld, tag=%s, code=%d)",
                                  item.tid, item.tag, code);
                            _eventListener->onEvent(MG_LOGGER_STATUS_WRITE,
                                                      "Failed to write log header");
                            break;
                        case CLOGAN_WRITE_FAIL_MALLOC:
                            ALOGE("MGLogger::run - Failed to write log due to malloc error (tid=%lld, tag=%s, code=%d)",
                                  item.tid, item.tag, code);
                            _eventListener->onEvent(MG_LOGGER_STATUS_WRITE,
                                                      "Failed to write log due to malloc error");
                            break;
                        case CLOGAN_WRITE_FAIL_PARAM:
                            ALOGE("MGLogger::run - Failed to write log due to parameter error (tid=%lld, tag=%s, code=%d)",
                                  item.tid, item.tag, code);
                            _eventListener->onEvent(MG_LOGGER_STATUS_WRITE,
                                                      "Failed to write log due to parameter error");
                            break;
                        default:
                            ALOGE("MGLogger::run - Unknown write result (tid=%lld, tag=%s, code=%d)",
                                  item.tid, item.tag, code);
                            _eventListener->onEvent(MG_LOGGER_STATUS_WRITE,
                                                      "Unknown write result");
                            break;
                    }
                }
                mBatchBuf.clear();
                mLastFlushTs = now;
                SDL_UnlockMutex(m_mutex);
            }
        }
        SDL_LockMutex(m_mutex);
        //处理残留日志
        for (auto &item: mBatchBuf) {
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

    /**
     * 设置事件监听器
     * @param listener
     */
    void MGLogger::SetOnEventListener(std::shared_ptr<OnEventListener> listener) {
        SDL_LockMutex(m_mutex);
        _eventListener = std::move(listener);
        SDL_UnlockMutex(m_mutex);
    }

    /**
     * 消息处理线程函数
     * @param arg
     * @return
     */
    int MGLogger::runMessageThread() {
        ALOGD("MGLogger::runMessageThread - Entering message consumption loop");
        while (alive) {
            if (m_message_tid == nullptr) {
                ALOGW("MGLogger::runMessageThread - Message thread is null, exiting loop");
                break;
            }
            // 阻塞等待获取一条消息
            std::shared_ptr<MGMessage> msg = mLogger->getMessage();
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
            case MG_LOGGER_STATUS_FORK_FAILED:
                ALOGE("MGLogger::handleMessage - Fork failed: %s", msg->msg.c_str());
                if (_eventListener) {
                    _eventListener->onEvent(MG_LOGGER_STATUS_FORK_FAILED, msg->msg.c_str());
                }
                break;
            case MG_LOGGER_STATUS_LOGCAT_UNAVAILABLE:
                ALOGE("MGLogger::handleMessage - Logcat unavailable: %s", msg->msg.c_str());
                if (_eventListener) {
                    _eventListener->onEvent(MG_LOGGER_STATUS_LOGCAT_UNAVAILABLE, msg->msg.c_str());
                }
                break;
            case MG_LOGGER_STATUS_PIPE_OPEN_FAILED:
                ALOGE("MGLogger::handleMessage - Pipe open failed: %s", msg->msg.c_str());
                if (_eventListener) {
                    _eventListener->onEvent(MG_LOGGER_STATUS_PIPE_OPEN_FAILED, msg->msg.c_str());
                }
                break;
            case MG_LOGGER_STATUS_FORK_EXITED:
                ALOGI("MGLogger::handleMessage - Fork exited with error: %s", msg->msg.c_str());
                if (_eventListener) {
                    ALOGD("MGLogger::handleMessage - Notifying event listener about fork exit");
                    _eventListener->onEvent(MG_LOGGER_STATUS_FORK_EXITED, msg->msg.c_str());
                }
                switchToHookMode();
                break;
            case MG_LOGGER_STATUS_FORK_TIMEOUT:
                ALOGE("MGLogger::handleMessage - Fork timeout: %s", msg->msg.c_str());
                if (_eventListener) {
                    ALOGD("MGLogger::handleMessage - Notifying event listener about fork timeout");
                    _eventListener->onEvent(MG_LOGGER_STATUS_FORK_TIMEOUT, msg->msg.c_str());
                }
                switchToHookMode();
                break;
            case MG_LOGGER_STATUS_FORK_STARTED:
            default:
                ALOGW("MGLogger::handleMessage - Unknown message type: %d", msg->what);
                break;
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

    int MGLogger::switchToHookMode() {
        ALOGI("MGLogger::switchToHookMode - Switching to Hook mode");
        stopThreads();
        if (mLogger) {
            mLogger->stop();
            mLogger.reset();
        }
        mLogger = BaseLogger::CreateLogger(LOGGER_TYPE_HOOK);
        if (!mLogger) {
            ALOGE("MGLogger::switchToHookMode - Failed to create Hook logger");
            return MG_LOGGER_CREATE_FAILED;
        }

        if (!mBlackList.empty()) {
            mLogger->setBlackList(mBlackList);
        }

        int result = mLogger->init();
        if (result != MG_OK) {
            ALOGE("MGLogger::switchToHookMode - ILogger initialization failed (code=%d)", result);
            return result;
        }
        result = mLogger->start();
        if (result != MG_OK) {
            ALOGE("MGLogger::switchToHookMode - ILogger start failed (code=%d)", result);
            return result;
        }
        if (!startThreads()) {
            ALOGE("MGLogger::switchToHookMode - Failed to start threads");
            return MG_LOGGER_CREATE_WORKER_THREAD_FAILED;
        }
        return MG_OK;
    }

    int MGLogger::reWrite(MGLog *log) {
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
        if (mCacheFilePath.empty()) {
            ALOGE("MGLogger::reWrite - Cache file path is empty, cannot rewrite log");
            return MG_ERROR;
        }
        std::map<std::string, long long> fileInfo =
                utils::LoggerUtils::collectFileInfo(mCacheFilePath.c_str());

        long long totalSize = 0;
        for (const auto &kv: fileInfo) {
            totalSize += kv.second;
        }

        long long freeSpace = static_cast<long long>(mMaxSDCardFileSize) - totalSize;
        while (freeSpace < mMaxSingleFileSize && !fileInfo.empty()) {
            auto oldest = std::min_element(
                    fileInfo.begin(), fileInfo.end(),
                    [](const std::pair<const std::string, long long> &a,
                       const std::pair<const std::string, long long> &b) {
                        return utils::LoggerUtils::parseTsFromFileName(a.first) <
                               utils::LoggerUtils::parseTsFromFileName(b.first);
                    });
            if (oldest == fileInfo.end()) {
                break;
            }
            std::string fullPath = mCacheFilePath + "/" + oldest->first;
            if (std::remove(fullPath.c_str()) == 0) {
                totalSize -= oldest->second;
            } else {
                ALOGE("MGLogger::reWrite - Failed to delete file: %s", fullPath.c_str());
            }
            fileInfo.erase(oldest);
            freeSpace = static_cast<long long>(mMaxSDCardFileSize) - totalSize;
        }

        const char *fileName = utils::LoggerUtils::toCString(utils::LoggerUtils::nowMs());
        clogan_open(fileName);
        int is_main = (log->tid == getpid()) ? 1 : 0;
        int code = clogan_write(0,
                                log->msg,
                                log->ts,
                                const_cast<char *>(log->tag),
                                log->tid,
                                is_main);
        return code;
    }

    int MGLogger::flush() {
        ALOGI("MGLogger::handleMessage - Flush requested");
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
        stopThreads();
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
#ifndef MGLOGGER_LOGGER_CORE_H
#define MGLOGGER_LOGGER_CORE_H

#include <string>
#include "ljjlogger/logan/clogan_core.h"  // CLogan API header
#include "clogan_status.h"
#include "message_queue.h"
#include "logger_base.h"
#include "logger_common.h"
#include "logger_listener.h"
#include "vector"
#include <list>
#include "logger_status.h"
#include "logger_utils.h"
#include <chrono>
#include <cstdint>
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/17
 * Time： 20:20
 */

namespace MGLogger {

    static constexpr size_t BATCH_SIZE = 64;          // 一批 64 条
    static constexpr uint32_t FLUSH_INTERVAL_MS = 200; // 或 200 ms

    class MGLogger {
    public:
        MGLogger();

        ~MGLogger();

        int init(const char *cache_path,
                 const char *dir_path,
                 int log_cache_s,
                 int max_file,
                 int max_sdcard_size,
                 const char *key16,
                 const char *iv16);

        int open(const char *file_name);

        int write(int flag, const char *log, long long local_time,
                  const char *thread_name, long long thread_id, int is_main);

        int flush();

        void debug(int debug);

        void stop();

        int  mergeCompressedLogs(const char *file_name);

        void setBlackList(const std::list<std::string> &blackList);

        static int threadFunc(void *arg);

        int run();

        static int messageThreadFunc(void *arg);

        int runMessageThread();

        virtual void SetOnEventListener(std::shared_ptr<OnEventListener> listener);

    private:
        SDL_Thread *createEnqueueTh();

        SDL_Thread *createMessageTh();

        /**
         * Stop worker and message threads
         */
        void stopThreads();

        /**
         * Start worker and message threads
         * @return true if both threads created successfully
         */
        bool startThreads();

        void handleMessage(const std::shared_ptr<MGMessage> &msg);

        int write(MGLog *log);

        int reWrite(MGLog *log);

        int switchToHookMode();


    private:
        std::shared_ptr<ILogger> mLogger{nullptr};
        std::vector<MGLog> mBatchBuf;                     // 批量缓存
        uint64_t mLastFlushTs{0};                         // 上次批量写入时间
        std::shared_ptr<OnEventListener> _eventListener{nullptr};
        SDL_Thread *m_worker_tid{nullptr};
        SDL_Thread _m_worker_thread{};
        SDL_Thread *m_message_tid{nullptr};
        SDL_Thread _m_message_thread{};
        SDL_mutex *m_mutex{nullptr};
        SDL_cond *m_cond{nullptr};
        bool running{false};
        bool alive{false};
        int mMaxSingleFileSize{0}; // 单个文件最大大小
        int mMaxSDCardFileSize{0};
        std::string mCacheFilePath; // 文件路径
        std::list<std::string> mBlackList{};               // 当前黑名单
    };
}

#endif //MGLOGGER_LOGGER_CORE_H

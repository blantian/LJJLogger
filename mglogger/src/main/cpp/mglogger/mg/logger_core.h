//
// Created by sky blue on 2025/7/17.
//

#ifndef MGLOGGER_LOGGER_CORE_H
#define MGLOGGER_LOGGER_CORE_H


#include <asm/unistd-common.h>
#include <string>
#include "mglogger/logan/clogan_core.h"  // CLogan API header
#include "clogan_status.h"
#include "message_queue.h"
#include "logger_base.h"
#include "logger_common.h"
#include "logger_listener.h"
#include "vector"
#include "logger_status.h"

#include <chrono>
#include <cstdint>

namespace MGLogger {

    static constexpr size_t BATCH_SIZE = 64;          // 一批 64 条
    static constexpr uint32_t FLUSH_INTERVAL_MS = 1000; // 或 1 秒

    class MGLogger {
    public:
        MGLogger();

        ~MGLogger();

        int init(const char *cache_path,
                 const char *dir_path,
                 int log_cache_s,
                 int max_file,
                 const char *key16,
                 const char *iv16);

        int open(const char *file_name);

        int write(int flag, const char *log, long long local_time,
                  const char *thread_name, long long thread_id, int is_main);

        int write(MGLog *log);

        int flush();

        void debug(int debug);

        void stop();

        void setBlackList(const std::list<std::string> &blackList);

        static int threadFunc(void *arg);

        int run();

        static int messageThreadFunc(void *arg);

        int runMessageThread();

        virtual void SetOnEventListener(std::shared_ptr<OnEventListener> listener);

    private:
        SDL_Thread *createEnqueueTh();

        SDL_Thread *createMessageTh();

        void handleMessage(const std::shared_ptr<MGMessage> &msg);

        static inline uint64_t nowMs() {
            using namespace std::chrono;
            return duration_cast<milliseconds>(
                    steady_clock::now().time_since_epoch()
            ).count();          // 单位：毫秒
        }

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
    };
}

#endif //MGLOGGER_LOGGER_CORE_H

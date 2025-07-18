//
// Created by sky blue on 2025/7/17.
//

#ifndef MGLOGGER_MGLOGGER_H
#define MGLOGGER_MGLOGGER_H


#include "logger_hook.h"
#include <asm/unistd-common.h>
#include <string>
#include "mglogger/logan/clogan_core.h"  // CLogan API header
#include "clogan_status.h"

namespace MGLogger {

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

        static int threadFunc(void *arg);

        int run();

    private:
        SDL_Thread * createEnqueueTh();
    private:
        std::shared_ptr<LoggerHook> loggerHook{nullptr};
        SDL_Thread *m_worker_tid{nullptr};
        SDL_Thread _m_worker_thread{};
        SDL_mutex *m_mutex{nullptr};
        SDL_cond *m_cond{nullptr};
        bool running{false};
    };
}

#endif //MGLOGGER_MGLOGGER_H

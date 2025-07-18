/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 15:16
 */


#ifndef MGLOGGER_LOGGER_QUEUE_H
#define MGLOGGER_LOGGER_QUEUE_H

#include "list"

#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#include "sdl_thread.h"
#include "sdl_mutex.h"
#ifdef __cplusplus
}
#endif

typedef struct Log {
    long long tid;      // Thread ID
    char tag[64];       // MGLog tag
    char msg[1024];     // MGLog message
    long long ts;       // Timestamp in milliseconds
} MGLog;

namespace MGLogger {

    class LoggerNode;

    class LoggerQueue {
    public:
        explicit LoggerQueue(size_t m_maxCapacity = 1000);

        ~LoggerQueue();

        void enqueue(MGLog *log, int tag = 0);

        int dequeue(MGLog *log);

        inline int getSize() {
            return logList.size();
        }

        inline bool isAbort() {
            return (abort_request != 0);
        }

        void abort();

        void clear();

    private:


    private:
        SDL_mutex *m_mutex{nullptr};
        SDL_cond *m_cond{nullptr};
        std::list<std::shared_ptr<LoggerNode>> logList;
        const size_t m_maxCapacity;
        int abort_request{0};
    };

    class LoggerNode {
    public:
        LoggerNode() = default;
        ~LoggerNode() = default;

    private:
        MGLog log{};
        friend LoggerQueue;
        int tag = 0; // log tag, 0: print, 1: write, 2: vprint, 3: buf_write, 4: assert
    };
}

#endif //MGLOGGER_LOGGER_QUEUE_H

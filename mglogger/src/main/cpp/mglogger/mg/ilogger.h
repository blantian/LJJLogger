//
// Created by sky blue on 2025/7/17.
//

#ifndef MGLOGGER_ILOGGER_H
#define MGLOGGER_ILOGGER_H

#include <memory>
#include <unistd.h>
#include "logger_queue.h"
#include "string"
#include "message_queue.h"
#include <unordered_set>

#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#include "sdl_android_jni.h"
#ifdef __cplusplus
}
#endif

namespace MGLogger {

    class ILogger {
    public:


        virtual int init() = 0;

        virtual void writeLog(MGLog *log, int tag) = 0;

        virtual void enqueue(MGLog *log, int tag) = 0;

        virtual int dequeue(MGLog *log) = 0;

        virtual int start() = 0;

        virtual void stop() = 0;

        virtual std::shared_ptr<MGMessage> getMessage() = 0;

        virtual void setBlackList(const std::list<std::string> &blackList) = 0;

        virtual void setLogcatArgs(const std::vector<std::string> &args) = 0;


    };
}

#endif //MGLOGGER_ILOGGER_H

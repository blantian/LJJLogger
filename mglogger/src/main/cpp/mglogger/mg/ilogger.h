//
// Created by sky blue on 2025/7/17.
//

#ifndef MGLOGGER_ILOGGER_H
#define MGLOGGER_ILOGGER_H

namespace MGLogger {
    class ILogger {
    public:
        virtual void init() = 0;

        virtual void writeLog(MGLog *log, int tag) = 0;

        virtual void enqueue(MGLog *log, int tag) = 0;

        virtual int dequeue(MGLog *log) = 0;

        virtual void stop() = 0;

    };
}

#endif //MGLOGGER_ILOGGER_H

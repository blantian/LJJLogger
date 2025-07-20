//
// Created by sky blue on 2025/7/18.
//

#ifndef MGLOGGER_LOGGER_LISTENER_H
#define MGLOGGER_LOGGER_LISTENER_H

namespace MGLogger {

    class OnEventListener {
    public:
        virtual void OnEvent(int what, const char *cmd) = 0;
    };
}

#endif //MGLOGGER_LOGGER_LISTENER_H

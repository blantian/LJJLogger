//
// Created by sky blue on 2025/7/18.
//

#ifndef MGLOGGER_LOGGER_LISTENER_H
#define MGLOGGER_LOGGER_LISTENER_H

#include <jni.h>
#include <memory>

namespace MGLogger {

    class OnEventListener {
    public:
        virtual void onEvent(int what, const char *cmd) = 0;
    };

    std::shared_ptr<OnEventListener> createJniEventListener(JNIEnv *env, jobject thiz);
}

#endif //MGLOGGER_LOGGER_LISTENER_H

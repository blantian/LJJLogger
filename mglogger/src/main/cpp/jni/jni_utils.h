
#ifndef MGLOGGER_SDL_JNI_UTILS_H
#define MGLOGGER_SDL_JNI_UTILS_H

#include <jni.h>
#include "sdl_log.h"

constexpr const char* EXCEPTION_BASE = "java/lang/Exception";
constexpr const char* EXCEPTION_ARGUMENT = "java/lang/IllegalArgumentException";
constexpr const char* EXCEPTION_STATE = "java/lang/IllegalStateException";

int inline JNI_ThrowException(JNIEnv* env, const char* className, const char* msg) {
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        env->ExceptionClear();

        if (exception != nullptr) {
            ALOGW("Discarding pending exception (%s) to throw", className);
            env->DeleteLocalRef(exception);
        }
    }

    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == nullptr) {
        ALOGE("Unable to find exception class %s", className);
        /* ClassNotFoundException now pending */
        goto fail;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        ALOGE("Failed throwing '%s' '%s'", className, msg);
        /* an exception, most likely OOM, will now be pending */
        goto fail;
    }

    return 0;
    fail:
    if (exceptionClass)
        env->DeleteLocalRef(exceptionClass);
    return -1;
}

#endif //MGLOGGER_SDL_JNI_UTILS_H

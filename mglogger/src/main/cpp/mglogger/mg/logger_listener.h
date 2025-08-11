#ifndef MGLOGGER_LOGGER_LISTENER_H
#define MGLOGGER_LOGGER_LISTENER_H

#include <jni.h>
#include <memory>
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/27
 * Time： 10:58
 *
 */
namespace MGLogger {

    class OnEventListener {
    public:
        virtual void onEvent(int what, const char *cmd) = 0;
    };

    std::shared_ptr<OnEventListener> createJniEventListener(JNIEnv *env, jobject thiz);
}

#endif //MGLOGGER_LOGGER_LISTENER_H

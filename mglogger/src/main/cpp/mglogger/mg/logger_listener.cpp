#include "logger_listener.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "external/j4a/logger/OnEventListener.h"
#include "external/sdl/sdl_android_jni.h"
#ifdef __cplusplus
}
#endif


namespace MGLogger {

class JniEventListener : public OnEventListener {
public:
    JniEventListener(JNIEnv *env, jobject thiz) {
        m_thiz = J4A_NewGlobalRef__catchAll(env, thiz);
    }

    ~JniEventListener()  {
        if (!m_thiz) return;
        JNIEnv *env = nullptr;
        if (SDL_JNI_SetupThreadEnv(&env) == JNI_OK) {
            J4A_DeleteGlobalRef__p(env, &m_thiz);
        }
    }

    void onEvent(int what, const char *cmd) override {
        if (!m_thiz) return;
        JNIEnv *env = nullptr;
        if (SDL_JNI_SetupThreadEnv(&env) != JNI_OK) return;
        J4AC_com_mgtv_logger_kt_log_MGLoggerJni__onLoggerStatus__withCString__catchAll(env, m_thiz, what, cmd ? cmd : "");
    }

private:
    jobject m_thiz{nullptr};
};

std::shared_ptr<OnEventListener> createJniEventListener(JNIEnv *env, jobject thiz) {
    return std::make_shared<JniEventListener>(env, thiz);
}

} // namespace MGLogger


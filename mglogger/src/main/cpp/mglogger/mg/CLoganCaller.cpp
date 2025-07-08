/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/7
 * Time： 23:11
 */

#include "LoggerHookConfig.h"
#include <jni.h>
#include <android/log.h>
#include <ctime>
#include "CloganCaller.h"
#include "mglogger/logan/clogan_core.h"
#include <sys/syscall.h>

static JavaVM* gJvm = nullptr;
static jclass  gJniClass = nullptr;         // com/xxx/MGLoggerJni

/* 判断当前线程是否主线程 */
static inline int is_main_thread() {
    return (getpid() == (pid_t)syscall(__NR_gettid)) ? 1 : 0;
}

static JNIEnv* ensureEnv(bool* attached){
    *attached = false;
    JNIEnv* env = nullptr;
    if (gJvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK){
        if (gJvm->AttachCurrentThread(&env,nullptr) == 0){
            *attached = true;
        }
    }
    return env;
}

static void detachIf(bool attached){
    if (attached) gJvm->DetachCurrentThread();
}

extern "C" void clogan_bridge_init(JavaVM* vm, jclass cls){
    gJvm = vm;
    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    gJniClass = (jclass)env->NewGlobalRef(cls);
}

int clogan_bridge_write(int prio,const char* tag,const char* msg){

    char logBuf[1200];
    snprintf(logBuf, sizeof(logBuf),
             "[p:%d][%s] %s",
             prio,
             tag ? tag : "(null)",
             msg ? msg : "");

    long long ts = static_cast<long long>(time(nullptr)) * 1000LL;

    return clogan_write(0,
                        logBuf,
                        ts,
                        "bridge", // tag 占位
                        static_cast<long long>(syscall(__NR_gettid)),
                        is_main_thread());
}

void clogan_bridge_notifyHookFail(int code){
    bool attached;
    JNIEnv* env = ensureEnv(&attached);
    if (!env) return;
    jmethodID mid = env->GetStaticMethodID(gJniClass,"onHookFail","(I)V");
    if (mid) env->CallStaticVoidMethod(gJniClass, mid, code);
    detachIf(attached);
}

void clogan_bridge_notifyWriteFail(int code, const char* tag){
    bool attached;
    JNIEnv* env = ensureEnv(&attached);
    if (!env) return;
    jmethodID mid = env->GetStaticMethodID(gJniClass,"onWriteFail","(ILjava/lang/String;)V");
    jstring jtag = tag? env->NewStringUTF(tag): nullptr;
    if (mid) env->CallStaticVoidMethod(gJniClass, mid, code, jtag);
    if (jtag) env->DeleteLocalRef(jtag);
    detachIf(attached);
}

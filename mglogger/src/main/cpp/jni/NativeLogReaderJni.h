#ifndef MGLOGGER_NATIVELOGREADERJNI_H
#define MGLOGGER_NATIVELOGREADERJNI_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeGetSystemLog(JNIEnv *env,
                                                           jobject thiz);

#ifdef __cplusplus
}
#endif

#endif //MGLOGGER_NATIVELOGREADERJNI_H

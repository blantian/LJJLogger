#ifndef MGLOGGER_LOGCAT_COLLECTOR_H
#define MGLOGGER_LOGCAT_COLLECTOR_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeStartLogcatCollector(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jobjectArray blacklist);

#ifdef __cplusplus
}
#endif

#endif //MGLOGGER_LOGCAT_COLLECTOR_H

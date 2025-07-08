#ifndef ANDROID_NOVA_LOGAN_CLOGAN_PROTOCOL_H_H
#define ANDROID_NOVA_LOGAN_CLOGAN_PROTOCOL_H_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <jni.h>
#include "mglogger/logan/clogan_core.h"

/**
 * JNI write interface
 */
JNIEXPORT jint JNICALL
Java_com_mgtv_logger_CLoganProtocol_clogan_1write(JNIEnv *env, jobject thiz, jint flag,
                                                    jstring log, jlong local_time,
                                                    jstring thread_name, jlong thread_id,
                                                    jint is_main);
/**
 * JNI init interface
 */
JNIEXPORT jint JNICALL
Java_com_mgtv_logger_CLoganProtocol_clogan_1init(JNIEnv *env, jobject thiz, jstring cache_path,
                                                   jstring dir_path, jint max_file,
                                                   jstring encrypt_key_16, jstring encrypt_iv_16);

/**
 * JNI open interface
 */
JNIEXPORT jint JNICALL
Java_com_mgtv_logger_CLoganProtocol_clogan_1open(JNIEnv *env, jobject thiz, jstring file_name);

/**
 * JNI flush interface
 */
JNIEXPORT void JNICALL
Java_com_mgtv_logger_CLoganProtocol_clogan_1flush(JNIEnv *env, jobject thiz);

/**
 * JNI debug interface
 */
JNIEXPORT void JNICALL
Java_com_mgtv_logger_CLoganProtocol_clogan_1debug(JNIEnv *env, jobject thiz, jboolean is_debug);



/**
 * JNI mglogger write interface
 */
JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1init(JNIEnv *env, jobject thiz,
                                                       jstring cache_path, jstring dir_path,
                                                       jint max_file, jstring encrypt_key16,
                                                       jstring encrypt_iv16);


JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1open(JNIEnv *env, jobject thiz,
                                                               jstring file_name);

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1debug(JNIEnv *env, jobject thiz,
                                                        jboolean is_debug);


JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1write(JNIEnv *env, jobject thiz, jint flag,
                                                        jstring log, jlong local_time,
                                                        jstring thread_name, jlong thread_id,
                                                        jint is_main);


JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1flush(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeStartLogcatCollector(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jobjectArray blacklist);

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeHookLogs(JNIEnv *env, jobject thiz);


#ifdef __cplusplus
}
#endif

#endif //ANDROID_NOVA_LOGAN_CLOGAN_PROTOCOL_H_H





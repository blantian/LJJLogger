//
// Created by sky blue on 2025/7/17.
//

#include "jni.h"
#include "logger_core.h"
#include "logger_listener.h"
#include "logger_common.h"

static JavaVM *g_vm = nullptr;

namespace MGLogger {

    std::shared_ptr<MGLogger> logger = nullptr;

    extern "C"
    JNIEXPORT jint JNICALL
    Java_com_mgtv_logger_kt_log_MGLoggerJni_LoggerInit(JNIEnv *env,
                                                       jobject thiz,
                                                       jstring cache_path,
                                                       jstring dir_path,
                                                       jint log_cache_s,
                                                       jint max_file,
                                                       jstring encrypt_key16,
                                                       jstring encrypt_iv16) {
        const char *dir_path_ = env->GetStringUTFChars(dir_path, 0);
        const char *cache_path_ = env->GetStringUTFChars(cache_path, 0);
        const char *encrypt_key16_ = env->GetStringUTFChars(encrypt_key16, 0);
        const char *encrypt_iv16_ = env->GetStringUTFChars(encrypt_iv16, 0);

        env->GetJavaVM(&g_vm);

        logger = std::make_shared<MGLogger>();
        int code = logger->init(cache_path_,
                                dir_path_,
                                log_cache_s,
                                max_file,
                                encrypt_key16_,
                                encrypt_iv16_);
        if (code == MG_OK) {
            logger->SetOnEventListener(createJniEventListener(env, thiz));
        }

        env->ReleaseStringUTFChars(dir_path, dir_path_);
        env->ReleaseStringUTFChars(cache_path, cache_path_);
        env->ReleaseStringUTFChars(encrypt_key16, encrypt_key16_);
        env->ReleaseStringUTFChars(encrypt_iv16, encrypt_iv16_);

        return code;
    }

    extern "C"
    JNIEXPORT jint JNICALL
    Java_com_mgtv_logger_kt_log_MGLoggerJni_LoggerOpen(JNIEnv *env,
                                                       jobject thiz,
                                                       jstring file_name) {
        const char *file_name_ = env->GetStringUTFChars(file_name, 0);

        jint code = logger->open(file_name_);

        env->ReleaseStringUTFChars(file_name, file_name_);
        return code;
    }

    extern "C"
    JNIEXPORT void JNICALL
    Java_com_mgtv_logger_kt_log_MGLoggerJni_LoggerDebug(JNIEnv *env, jobject thiz,
                                                        jboolean is_debug) {
        int i = 1;
        if (!is_debug) {
            i = 0;
        }
        logger->debug(i);
    }

    extern "C"
    JNIEXPORT jint JNICALL
    Java_com_mgtv_logger_kt_log_MGLoggerJni_LoggerWrite(JNIEnv *env,
                                                        jobject thiz,
                                                        jint flag,
                                                        jstring log,
                                                        jlong local_time,
                                                        jstring thread_name,
                                                        jlong thread_id,
                                                        jint is_main) {
        const char *log_ = env->GetStringUTFChars(log, 0);
        const char *thread_name_ = env->GetStringUTFChars(thread_name, 0);

        jint code = logger->write(flag,
                                  (char *) log_,
                                  local_time,
                                  (char *) thread_name_,
                                  thread_id,
                                  is_main);

        env->ReleaseStringUTFChars(log, log_);
        env->ReleaseStringUTFChars(thread_name, thread_name_);
        return code;
    }

    extern "C"
    JNIEXPORT void JNICALL
    Java_com_mgtv_logger_kt_log_MGLoggerJni_LoggerFlush(JNIEnv *env, jobject thiz) {
        logger->flush();
    }


    static void on_logcat_fail_callback() {
        if (g_vm == nullptr) return;
        JNIEnv *env = nullptr;
        if (g_vm->AttachCurrentThread(&env, nullptr) != 0) return;
        jclass cls = env->FindClass("com/mgtv/logger/kt/log/MGLoggerJni");
        if (cls != nullptr) {
            jmethodID mid = env->GetStaticMethodID(cls, "onLogcatCollectorFail", "()V");
            if (mid != nullptr) {
                env->CallStaticVoidMethod(cls, mid);
            }
            env->DeleteLocalRef(cls);
        }
        g_vm->DetachCurrentThread();
    }

//    extern "C"
//    JNIEXPORT void JNICALL
//    Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeStartLogcatCollector(JNIEnv *env,
//                                                                       jobject thiz,
//                                                                       jobjectArray blacklist) {
//        int count = 0;
//        if (blacklist != nullptr) {
//            count = env->GetArrayLength(blacklist);
//        }
//        const char **list = nullptr;
//        if (count > 0) {
//            list = (const char **) malloc(sizeof(char *) * count);
//            for (int i = 0; i < count; ++i) {
//                jstring str = (jstring) env->GetObjectArrayElement(blacklist, i);
//                const char *tmp = env->GetStringUTFChars(str, 0);
//                list[i] = strdup(tmp);
//                env->ReleaseStringUTFChars(str, tmp);
//                env->DeleteLocalRef(str);
//            }
//        }
//
//        int ret = start_logreader(list, count, on_logcat_fail_callback);
//        if (ret < 0) {
//            on_logcat_fail_callback();
//        }
//        if (list) {
//            for (int i = 0; i < count; ++i) {
//                free((void *) list[i]);
//            }
//            free(list);
//        }
//    }
}


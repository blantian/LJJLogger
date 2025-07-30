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
    Java_com_mgtv_logger_log_LoggerNativeBridge_initLogger(JNIEnv *env,
                                                           jclass clazz,
                                                           jstring cache_path,
                                                           jstring dir_path,
                                                           jint log_cache_selector,
                                                           jint max_file,
                                                           jint max_sd_card_size,
                                                           jstring encrypt_key16,
                                                           jstring encrypt_iv16) {

        const char *dir_path_ = env->GetStringUTFChars(dir_path, nullptr);
        const char *cache_path_ = env->GetStringUTFChars(cache_path, nullptr);
        const char *encrypt_key16_ = env->GetStringUTFChars(encrypt_key16, nullptr);
        const char *encrypt_iv16_ = env->GetStringUTFChars(encrypt_iv16, nullptr);

        env->GetJavaVM(&g_vm);

        logger = std::make_shared<MGLogger>();
        int code = logger->init(cache_path_,
                                dir_path_,
                                log_cache_selector,
                                max_file,
                                max_sd_card_size,
                                encrypt_key16_,
                                encrypt_iv16_);
        if (code == MG_OK) {
            logger->SetOnEventListener(createJniEventListener(env, clazz));
        }

        env->ReleaseStringUTFChars(dir_path, dir_path_);
        env->ReleaseStringUTFChars(cache_path, cache_path_);
        env->ReleaseStringUTFChars(encrypt_key16, encrypt_key16_);
        env->ReleaseStringUTFChars(encrypt_iv16, encrypt_iv16_);

        return code;
    }

    extern "C"
    JNIEXPORT jint JNICALL
    Java_com_mgtv_logger_log_LoggerNativeBridge_LoggerOpen(JNIEnv *env, jobject thiz,
                                                           jstring file_name) {
        const char *file_name_ = env->GetStringUTFChars(file_name, nullptr);

        jint code = logger->open(file_name_);

        env->ReleaseStringUTFChars(file_name, file_name_);
        return code;
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_mgtv_logger_log_LoggerNativeBridge_LoggerDebug(JNIEnv *env, jobject thiz,
                                                            jboolean is_debug) {
        int i = 1;
        if (!is_debug) {
            i = 0;
        }
        logger->debug(i);
    }
    extern "C"
    JNIEXPORT jint JNICALL
    Java_com_mgtv_logger_log_LoggerNativeBridge_LoggerWrite(JNIEnv *env, jobject thiz, jint flag,
                                                            jstring log, jlong local_time,
                                                            jstring thread_name, jlong thread_id,
                                                            jint is_main) {
        const char *log_ = env->GetStringUTFChars(log, nullptr);
        const char *thread_name_ = env->GetStringUTFChars(thread_name, nullptr);

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
    Java_com_mgtv_logger_log_LoggerNativeBridge_LoggerFlush(JNIEnv *env, jobject thiz) {
        logger->flush();
    }
    extern "C"
    JNIEXPORT void JNICALL
    Java_com_mgtv_logger_log_LoggerNativeBridge_LoggerSetBlackList(JNIEnv *env, jobject thiz,
                                                                   jobjectArray list) {
        if (logger == nullptr) {
            return;
        }
        std::list<std::string> vec;
        if (list != nullptr) {
            jsize len = env->GetArrayLength(list);
            for (jsize i = 0; i < len; ++i) {
                auto item = (jstring) env->GetObjectArrayElement(list, i);
                const char *c_str = env->GetStringUTFChars(item, nullptr);
                vec.emplace_back(c_str);
                env->ReleaseStringUTFChars(item, c_str);
                env->DeleteLocalRef(item);
            }
        }
        logger->setBlackList(vec);
    }
}


#include "mglogger_jni.h"
#include "mg/Logreader.h"

static JavaVM *g_vm = NULL;

JNIEXPORT jint JNICALL
Java_com_mgtv_logger_java_CLoganProtocol_clogan_1write(JNIEnv *env, jobject instance, jint flag,
                                                  jstring log_, jlong local_time,
                                                  jstring thread_name_, jlong thread_id,
                                                  jint is_main) {
    const char *log = (*env)->GetStringUTFChars(env, log_, 0);
    const char *thread_name = (*env)->GetStringUTFChars(env, thread_name_, 0);

    jint code = (jint) clogan_write(flag, (char *) log, local_time, (char *) thread_name, thread_id,
                                    is_main);

    (*env)->ReleaseStringUTFChars(env, log_, log);
    (*env)->ReleaseStringUTFChars(env, thread_name_, thread_name);
    return code;

}

JNIEXPORT jint JNICALL
Java_com_mgtv_logger_java_CLoganProtocol_clogan_1init(JNIEnv *env, jobject instance,
                                                 jstring cache_path_,
                                                 jstring dir_path_, jint max_file,
                                                 jstring encrypt_key16_, jstring encrypt_iv16_) {
    const char *dir_path = (*env)->GetStringUTFChars(env, dir_path_, 0);
    const char *cache_path = (*env)->GetStringUTFChars(env, cache_path_, 0);
    const char *encrypt_key16 = (*env)->GetStringUTFChars(env, encrypt_key16_, 0);
    const char *encrypt_iv16 = (*env)->GetStringUTFChars(env, encrypt_iv16_, 0);

    jint code = (jint) clogan_init(cache_path, dir_path, max_file, encrypt_key16, encrypt_iv16);

    (*env)->ReleaseStringUTFChars(env, dir_path_, dir_path);
    (*env)->ReleaseStringUTFChars(env, cache_path_, cache_path);
    (*env)->ReleaseStringUTFChars(env, encrypt_key16_, encrypt_key16);
    (*env)->ReleaseStringUTFChars(env, encrypt_iv16_, encrypt_iv16);
    return code;
}

JNIEXPORT jint JNICALL
Java_com_mgtv_logger_java_CLoganProtocol_clogan_1open(JNIEnv *env, jobject instance,
                                                 jstring file_name_) {
    const char *file_name = (*env)->GetStringUTFChars(env, file_name_, 0);

    jint code = (jint) clogan_open(file_name);

    (*env)->ReleaseStringUTFChars(env, file_name_, file_name);
    return code;
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_java_CLoganProtocol_clogan_1flush(JNIEnv *env, jobject instance) {
    clogan_flush();
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_java_CLoganProtocol_clogan_1debug(JNIEnv *env, jobject instance,
                                                  jboolean is_debug) {
    int i = 1;
    if (!is_debug) {
        i = 0;
    }
    clogan_debug(i);
}







JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1init(JNIEnv *env,
                                                       jobject thiz,
                                                       jstring cache_path,
                                                       jstring dir_path,
                                                       jint max_file,
                                                       jstring encrypt_key16,
                                                       jstring encrypt_iv16) {
    const char *dir_path_ = (*env)->GetStringUTFChars(env, dir_path, 0);
    const char *cache_path_ = (*env)->GetStringUTFChars(env, cache_path, 0);
    const char *encrypt_key16_ = (*env)->GetStringUTFChars(env, encrypt_key16, 0);
    const char *encrypt_iv16_ = (*env)->GetStringUTFChars(env, encrypt_iv16, 0);

    jint code = (jint) clogan_init(dir_path_, cache_path_, max_file, encrypt_key16_, encrypt_iv16_);

    (*env)->ReleaseStringUTFChars(env, dir_path, dir_path_);
    (*env)->ReleaseStringUTFChars(env, cache_path, cache_path_);
    (*env)->ReleaseStringUTFChars(env, encrypt_key16, encrypt_key16_);
    (*env)->ReleaseStringUTFChars(env, encrypt_iv16, encrypt_iv16_);
    return code;
}

JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1open(JNIEnv *env,
                                                       jobject thiz,
                                                       jstring file_name) {
    const char *file_name_ = (*env)->GetStringUTFChars(env, file_name, 0);

    jint code = (jint) clogan_open(file_name_);

    (*env)->ReleaseStringUTFChars(env, file_name, file_name_);
    return code;
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1debug(JNIEnv *env, jobject thiz,
                                                        jboolean is_debug) {
    int i = 1;
    if (!is_debug) {
        i = 0;
    }
    clogan_debug(i);
}


JNIEXPORT jint JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1write(JNIEnv *env,
                                                        jobject thiz,
                                                        jint flag,
                                                        jstring log,
                                                        jlong local_time,
                                                        jstring thread_name,
                                                        jlong thread_id,
                                                        jint is_main) {
    const char *log_ = (*env)->GetStringUTFChars(env, log, 0);
    const char *thread_name_ = (*env)->GetStringUTFChars(env, thread_name, 0);

    jint code = (jint) clogan_write(flag,
                                    (char *) log_,
                                    local_time,
                                    (char *) thread_name_,
                                    thread_id,
                                    is_main);

    (*env)->ReleaseStringUTFChars(env, log, log_);
    (*env)->ReleaseStringUTFChars(env, thread_name, thread_name_);
    return code;
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_mglogger_1flush(JNIEnv *env, jobject thiz) {
    clogan_flush();
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

static void on_logcat_fail_callback() {
    if (g_vm == NULL) return;
    JNIEnv *env = NULL;
    if ((*g_vm)->AttachCurrentThread(g_vm, &env, NULL) != 0) return;
    jclass cls = (*env)->FindClass(env, "com/mgtv/logger/kt/log/MGLoggerJni");
    if (cls != NULL) {
        jmethodID mid = (*env)->GetStaticMethodID(env, cls, "onLogcatCollectorFail", "()V");
        if (mid != NULL) {
            (*env)->CallStaticVoidMethod(env, cls, mid);
        }
        (*env)->DeleteLocalRef(env, cls);
    }
    (*g_vm)->DetachCurrentThread(g_vm);
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeStartLogcatCollector(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jobjectArray blacklist) {
    int count = 0;
    if (blacklist != NULL) {
        count = (*env)->GetArrayLength(env, blacklist);
    }
    const char **list = NULL;
    if (count > 0) {
        list = (const char **) malloc(sizeof(char *) * count);
        for (int i = 0; i < count; ++i) {
            jstring str = (jstring) (*env)->GetObjectArrayElement(env, blacklist, i);
            const char *tmp = (*env)->GetStringUTFChars(env, str, 0);
            list[i] = strdup(tmp);
            (*env)->ReleaseStringUTFChars(env, str, tmp);
            (*env)->DeleteLocalRef(env, str);
        }
    }
    int ret = start_logreader(list, count, on_logcat_fail_callback);
    if (ret < 0) {
        on_logcat_fail_callback();
    }
    if (list) {
        for (int i = 0; i < count; ++i) {
            free((void *) list[i]);
        }
        free(list);
    }
}

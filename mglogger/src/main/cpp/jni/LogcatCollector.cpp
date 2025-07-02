#include "LogcatCollector.h"
#include "clogan_core.h"
#include "base_util.h"
#include <pthread.h>
#include <vector>
#include <string>
#include <atomic>
#include <unistd.h>
#include <cstdio>
#include <cstring>

static pthread_t g_logcat_thread;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static std::atomic<int> g_running(0);
static std::vector<std::string> g_blacklist;
static JavaVM *g_vm = nullptr;
static jobject g_obj = nullptr;

static bool is_blacklisted(const char *tag) {
    if (!tag) return false;
    for (const auto &item : g_blacklist) {
        if (item == tag) return true;
    }
    return false;
}

static std::string extract_tag(const char *line) {
    const char *colon = strchr(line, ':');
    if (!colon) return "";
    const char *p = colon;
    while (p > line && *p != ' ') {
        --p;
    }
    if (*p == ' ') ++p;
    return std::string(p, colon - p);
}

static void callback_fail() {
    if (!g_vm || !g_obj) return;
    JNIEnv *env = nullptr;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) return;
    jclass cls = env->GetObjectClass(g_obj);
    if (cls) {
        jmethodID mid = env->GetMethodID(cls, "onLogcatCollectorFail", "()V");
        if (mid) env->CallVoidMethod(g_obj, mid);
        env->DeleteLocalRef(cls);
    }
    g_vm->DetachCurrentThread();
}

static void *logcat_loop(void *) {
    int retry = 0;
    while (g_running.load()) {
        FILE *pipe = popen("logcat -v threadtime", "r");
        if (!pipe) {
            if (++retry >= 3) {
                callback_fail();
                g_running.store(0);
                break;
            }
            sleep(1);
            continue;
        }
        retry = 0;
        char line[1024];
        while (g_running.load() && fgets(line, sizeof(line), pipe)) {
            std::string tag = extract_tag(line);
            if (is_blacklisted(tag.c_str())) continue;
            clogan_write(0, line, get_system_current_clogan(), (char *)"logcat", 0, 0);
        }
        pclose(pipe);
    }
    if (g_obj) {
        JNIEnv *env = nullptr;
        if (g_vm && g_vm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
            env->DeleteGlobalRef(g_obj);
            g_vm->DetachCurrentThread();
        }
        g_obj = nullptr;
    }
    return nullptr;
}

JNIEXPORT void JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeStartLogcatCollector(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jobjectArray blacklist) {
    pthread_mutex_lock(&g_mutex);
    if (g_running.load()) {
        pthread_mutex_unlock(&g_mutex);
        return;
    }

    g_blacklist.clear();
    if (blacklist) {
        jsize len = env->GetArrayLength(blacklist);
        for (jsize i = 0; i < len; ++i) {
            jstring item = (jstring) env->GetObjectArrayElement(blacklist, i);
            const char *str = env->GetStringUTFChars(item, 0);
            g_blacklist.push_back(str ? str : "");
            env->ReleaseStringUTFChars(item, str);
            env->DeleteLocalRef(item);
        }
    }

    env->GetJavaVM(&g_vm);
    g_obj = env->NewGlobalRef(thiz);
    g_running.store(1);
    pthread_create(&g_logcat_thread, nullptr, logcat_loop, nullptr);
    pthread_mutex_unlock(&g_mutex);
}

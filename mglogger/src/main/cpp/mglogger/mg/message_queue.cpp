/**
 * Description:
 * Created by lantian 
 * Date: 2025/7/14
 * Time: 15:56
 *
 */
#include <jni.h>
#include "message_queue.h"
extern "C"{
#include "sdl_log.h"
#include "sdl_android_jni.h"
}

message_queue::message_queue(size_t cap)
        : m_mutex(nullptr),
          m_cond(nullptr),
          m_worker_tid(nullptr),
          m_stop(false),
          m_capacity(cap),
          m_callback(nullptr) {

    m_mutex = SDL_CreateMutex();
    if (!m_mutex) {
        ALOGE("Failed to create message queue mutex");
    }
    m_cond = SDL_CreateCond();

    if (!m_cond) {
        ALOGE("Failed to create message queue cond");
    }

    m_worker_tid = SDL_CreateThreadEx(&_m_worker_thread, &threadFunc, this, "logan_status_worker");

    if (!m_worker_tid) {
        ALOGE("Failed to create message queue worker thread");
    }
}

message_queue::~message_queue() {
    // 确保线程已退出
    stop();
    if (m_mutex) {
        SDL_DestroyMutex(m_mutex);
        m_mutex = nullptr;
    }
    if (m_cond) {
        SDL_DestroyCond(m_cond);
        m_cond = nullptr;
    }
}


void message_queue::stop() {
    SDL_LockMutex(m_mutex);
    m_stop = true;
    SDL_CondSignal(m_cond);  // 唤醒线程以退出
    SDL_UnlockMutex(m_mutex);
    if (m_worker_tid) {
        SDL_WaitThread(m_worker_tid, nullptr);
        m_worker_tid = nullptr;
    }
}

void message_queue::push(int code, const char* type) {
    SDL_LockMutex(m_mutex);
    // 将事件添加到队列
    bool wasEmpty = m_events.empty();
    m_events.emplace(code, type ? std::string(type) : std::string());
    if (wasEmpty) {
        SDL_CondSignal(m_cond);
    }
    SDL_UnlockMutex(m_mutex);
}

void message_queue::setCallback(void (*callback)(int, const char*)) {
    SDL_LockMutex(m_mutex);
    m_callback = callback;
    SDL_UnlockMutex(m_mutex);
}


int message_queue::threadFunc(void* data) {
    auto* self = static_cast<message_queue*>(data);
    return self ? self->run() : 0;
}

int message_queue::run() {
    // 独立回调线程循环处理状态事件，并调用 Java 层回调
    JNIEnv* env = nullptr;
#if defined(__ANDROID__)
//    env = static_cast<JNIEnv*>(SDL_JNI_GetJvm());
#endif
    SDL_LockMutex(m_mutex);
    while (true) {
        while (!m_stop && m_events.empty()) {
            SDL_CondWait(m_cond, m_mutex);
        }
        if (m_stop && m_events.empty()) {
            SDL_UnlockMutex(m_mutex);
            break;
        }
        // 取出一个事件进行处理
        StatusEvent event = m_events.front();
        m_events.pop();
        SDL_UnlockMutex(m_mutex);
        // 调用 Java LoganCallback.onStatus 回调（通过J4A）
//        if (env) {
//            J4AC_LoganCallback__onStatus__withCString(env, event.code, event.type.c_str());
//        }
        // 如果设置了本地回调函数，调用本地回调
        if (m_callback) {
            m_callback(event.code, event.type.c_str());
        }
        SDL_LockMutex(m_mutex);
    }
    // 分离 Java 线程（Detach），清理 JNI 环境
//    if (env) {
//        JavaVM* jvm = nullptr;
//        env->GetJavaVM(&jvm);
//        if (jvm) {
//            jvm->DetachCurrentThread();
//        }
//    }
    return 0;
}


/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/4
 * Time： 23:58
 */

#include <jni.h>
#include <string>
#include <dlfcn.h>
#include <android/log.h>
#include "xhook.h"
#include "Logreader.h"
#include "clogan_core.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <ctime>


static int (*orig_log_print)(int, const char*, const char*, ...) = nullptr;
static int (*orig_log_write)(int, const char*, const char*) = nullptr;
static int (*orig_log_buf_write)(int, int, const char*, const char*) = nullptr;
static int (*orig_log_vprint)(int, const char*, const char*, va_list) = nullptr;
static void (*orig_log_assert)(const char*, const char*, const char*, ...) = nullptr;


static int hook_log_print(int prio, const char* tag, const char* fmt, ...) {
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    char msgBuf[1024];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    long long ts = (long long)time(nullptr) * 1000LL;
    clogan_write(0, msgBuf, ts, (char*)"hook_print", (long long)syscall(__NR_gettid), 0);

    char debugMsg[1150];
    snprintf(debugMsg, sizeof(debugMsg),
             "__android_log_print called: prio=%d, tag=%s, msg=%s",
             prio, (tag ? tag : "(null)"), msgBuf);

    va_end(args);

    int result = 0;
    if (orig_log_vprint) {
        result = orig_log_vprint(prio, tag, fmt, args_copy);
    } else if (orig_log_print) {
        result = orig_log_print(prio, tag, fmt, args_copy);
    }
    va_end(args_copy);
    return result;
}


static int hook_log_write(int prio, const char* tag, const char* text) {

    char debugMsg[1150];
    snprintf(debugMsg, sizeof(debugMsg),
             "__android_log_write called: prio=%d, tag=%s, text=%s",
             prio, (tag ? tag : "(null)"), (text ? text : "(null)"));

    long long ts = (long long)time(nullptr) * 1000LL;
    if (text) {
        clogan_write(0, (char*)text, ts, (char*)"hook_write", (long long)syscall(__NR_gettid), 0);
    }
    int result = 0;
    if (orig_log_write) {
        result = orig_log_write(prio, tag, text);
    }
    return result;
}

static int hook_log_buf_write(int bufID, int prio, const char* tag, const char* text) {
    long long ts = (long long)time(nullptr) * 1000LL;
    if (text) {
        clogan_write(0, (char*)text, ts, (char*)"hook_buf", (long long)syscall(__NR_gettid), 0);
    }
    int result = 0;
    if (orig_log_buf_write) {
        result = orig_log_buf_write(bufID, prio, tag, text);
    }
    return result;
}

static int hook_log_vprint(int prio, const char* tag, const char* fmt, va_list ap) {
    va_list args_copy;
    va_copy(args_copy, ap);
    char msgBuf[1024];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args_copy);
    va_end(args_copy);

    long long ts = (long long)time(nullptr) * 1000LL;
    clogan_write(0, msgBuf, ts, (char*)"hook_vprint", (long long)syscall(__NR_gettid), 0);

    int result = 0;
    if (orig_log_vprint) {
        result = orig_log_vprint(prio, tag, fmt, ap);
    }
    return result;
}

static void hook_log_assert(const char* cond, const char* tag, const char* fmt, ...) {
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    char msgBuf[1024];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    va_end(args);

    long long ts = (long long)time(nullptr) * 1000LL;
    clogan_write(0, msgBuf, ts, (char*)"hook_assert", (long long)syscall(__NR_gettid), 0);

    if (orig_log_assert) {
        orig_log_assert(cond, tag, fmt, args_copy);
    }
    va_end(args_copy);
}

void hook_log(){
    // xHook 进行 PLT Hook，确保延迟加载的 so 中也能 hook 到
    xhook_register(".*liblog\\.so$", "__android_log_write", (void*)hook_log_write, (void**)&orig_log_write);
    xhook_register(".*liblog\\.so$", "__android_log_print", (void*)hook_log_print, (void**)&orig_log_print);
    xhook_register(".*liblog\\.so$", "__android_log_vprint", (void*)hook_log_vprint, (void**)&orig_log_vprint);
    xhook_register(".*liblog\\.so$", "__android_log_assert", (void*)hook_log_assert, (void**)&orig_log_assert);
    xhook_register(".*\\.so$", "__android_log_buf_write", (void*)hook_log_buf_write, (void**)&orig_log_buf_write);

    xhook_refresh(1);
}
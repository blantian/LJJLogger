/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/7
 * Time： 23:09
 */


#include "LoggerHook.h"
#include <xhook.h>
#include <android/log.h>
#include <dlfcn.h>
#include <mutex>
#include "LoggerHookConfig.h"
#include "CloganCaller.h"

static int  (*orig_print)(int,const char*,const char*,...)=nullptr;
static int  (*orig_write)(int,const char*,const char*)   =nullptr;
static int  (*orig_buf_write)(int,int,const char*,const char*)=nullptr;

static __thread bool gInHook = false;
static inline void forward_to_clogan(int prio,const char* tag,const char* text) {
    int code = clogan_bridge_write(prio, tag, text);   // 调用封装层
    if (code != 0)  clogan_bridge_notifyWriteFail(code, tag);
}

static int hook_print(int prio,const char* tag,const char* fmt,...) {
    if (gInHook)    goto CALL_ORIG;

    gInHook = true;
    char  msg[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    forward_to_clogan(prio, tag, msg);

#if LOGGER_HOOK_DEBUG
    orig_print ? orig_print(prio, tag, "%s", msg)
               : __android_log_print(prio, tag, "%s", msg);
#endif
    gInHook = false;
    return 0;

    CALL_ORIG:
    if (orig_print) {
        va_list ap2;
        va_start(ap2, fmt);
        int r = orig_print(prio, tag, fmt, ap2);
        va_end(ap2);
        return r;
    }
    return 0;
}

static int hook_write(int prio,const char* tag,const char* text){
    if (!gInHook){
        gInHook = true;
        forward_to_clogan(prio, tag, text);
#if LOGGER_HOOK_DEBUG
        if (orig_write) orig_write(prio, tag, text);
#endif
        gInHook = false;
        return 0;
    }
    return orig_write ? orig_write(prio, tag, text) : 0;
}

static int hook_buf_write(int buf,int prio,const char* tag,const char* text){
    if (!gInHook){
        gInHook = true;
        forward_to_clogan(prio, tag, text);
#if LOGGER_HOOK_DEBUG
        if (orig_buf_write) orig_buf_write(buf, prio, tag, text);
#endif
        gInHook = false;
        return 0;
    }
    return orig_buf_write ? orig_buf_write(buf, prio, tag, text) : 0;
}

static JavaVM* sJvm = nullptr;
static std::once_flag gOnce;

extern "C"
int logger_hook_init(JavaVM* jvm)
{
    std::call_once(gOnce, [&]{
        sJvm = jvm;
#if LOGGER_HOOK_DEBUG
        xhook_enable_debug(1);
#endif
        xhook_enable_sigsegv_protection(1);

        xhook_register(".*\\.so$", "__android_log_print",
                       (void*)hook_print, (void**)&orig_print);
        xhook_register(".*liblog\\.so$", "__android_log_write",
                       (void*)hook_write, (void**)&orig_write);
        xhook_register(".*liblog\\.so$", "__android_log_buf_write",
                       (void*)hook_buf_write, (void**)&orig_buf_write);

        int ret = xhook_refresh(0);
        if (ret != 0) {
            clogan_bridge_notifyHookFail(ret);
        }
        xhook_clear();
    });
    return 0;
}

void logger_hook_deinit(){
    /* 当前 xHook 不支持自动撤销 inline hook；如需热修复可重启进程 */
}

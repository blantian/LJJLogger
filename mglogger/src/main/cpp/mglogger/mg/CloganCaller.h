/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/7
 * Time： 23:20
 */


//  clogan_caller.h
//  仅声明与 clogan 交互 & JNI 回调相关的桥接接口
//  ---------------------------------------------------
#pragma once
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * 保存 JVM 指针与回调目标类（在 nativeInitHook 中调用）
 *
 * @param vm   JavaVM*，通常就是 JNI_OnLoad 收到的那个
 * @param cls  目标 Java 类 (com/yourpackage/MGLoggerJni)；必须是全局引用或
 *             调用方保证生命周期 ≥ 进程。建议直接传入 env->FindClass()
 *             返回值，在实现里转成 GlobalRef。
 */
void clogan_bridge_init(JavaVM* vm, jclass cls);

/**
 * 写日志到 clogan
 * @return 0 成功；其他为 clogan_write 的错误码
 */
int  clogan_bridge_write(int prio, const char* tag, const char* msg);

/* =========== 失败回调（由 native 内部主动调用，无需 Java 主动调用） ========== */

/** Hook 安装失败时调用，errorCode 为 xhook_refresh 的返回值 */
void clogan_bridge_notifyHookFail(int errorCode);

/** clogan_write 返回非 0 时调用 */
void clogan_bridge_notifyWriteFail(int errorCode, const char* tag);

#ifdef __cplusplus
}
#endif


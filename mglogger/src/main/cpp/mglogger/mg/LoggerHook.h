/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/7
 * Time： 23:09
 */


#pragma once
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  供 JNI / 其他 C 模块调用：一次性完成 Hook 安装
 *  成功返回 0；失败返回错误码并自动通过 onHookFail 回调 Java */
int logger_hook_init(JavaVM* jvm);

/*  主动关闭 & 释放（如热修复） */
void logger_hook_deinit();

#ifdef __cplusplus
}
#endif

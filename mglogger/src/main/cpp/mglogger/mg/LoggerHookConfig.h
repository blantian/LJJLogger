/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/7
 * Time： 23:12
 */


#pragma once
/* =========================================================
 *  商用/调试开关（编译时宏）
 *  DEBUG == 1  : 编译进所有调试日志 & xhook debug
 *  DEBUG == 0  : 生产包，无调试日志
 * ========================================================= */
#ifndef LOGGER_HOOK_DEBUG
#define LOGGER_HOOK_DEBUG 1           // 由 build.gradle 或 CMake -DLOGGER_HOOK_DEBUG=1 控制
#endif

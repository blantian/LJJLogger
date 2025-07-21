//
// Created by sky blue on 2025/7/18.
//

#ifndef MGLOGGER_LOGGER_STATUS_H
#define MGLOGGER_LOGGER_STATUS_H

#define MG_LOGGER_VERSION "1.0.0"
#define DEBUG_LOG 1

#define LOGGER_TYPE_HOOK 0 // 使用 Hook 方式记录日志
#define LOGGER_TYPE_FORK 1 // 使用 Fork 方式记录日志

#define OPEN_PRINT 1
#define OPEN_WRITE 0
#define OPEN_VPRINT 0
#define OPEN_BUF_WRITE 1
#define OPEN_ASSERT 0

#define MG_OK 0
#define MG_ERROR -1

#define LOG_DEBUG 'D'
#define LOG_INFO 'I'
#define LOG_WARN 'W'
#define LOG_ERROR 'E'
#define LOG_FATAL 'F'
#define LOG_VERBOSE 'V'
#define LOG_UNKNOWN 'U'

#define LOG_MAX_LENGTH 1024 // 日志最大长度

#define ANDROID_API_LEVEL 19 // Android API level

#define MG_LOGGER_LOG "MGLogger.zip" // 日志文件名



#define LOG_READ_TIME_OUT 20 // ms


#endif //MGLOGGER_LOGGER_STATUS_H

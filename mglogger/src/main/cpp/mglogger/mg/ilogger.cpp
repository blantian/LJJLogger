//
// Created by sky blue on 2025/7/20.
//

#include "ilogger.h"
#include "logger_hook.h"

using namespace MGLogger;
std::shared_ptr<ILogger> ILogger::logger = nullptr;

std::shared_ptr<ILogger> ILogger::CreateLogger(const int type) {
    ALOGD("%s: create %d", __func__, type);
    switch (type) {
        case LOGGER_TYPE_HOOK:
            // 使用 Hook 方式拦截日志
            ALOGD("ILogger::CreateLogger - Creating LoggerHook");
            if (logger == nullptr) {
                logger = std::make_shared<LoggerHook>();
                if (!logger) {
                    ALOGE("ILogger::CreateLogger - Failed to create LoggerHook");
                    return nullptr;
                }
            } else {
                ALOGD("ILogger::CreateLogger - LoggerHook already exists");
            }
            break;
        case LOGGER_TYPE_FORK:
            // 使用 Logan 方式记录日志
            ALOGD("ILogger::CreateLogger - Creating CLoganLogger");
            break;
        default:
            ALOGE("ILogger::CreateLogger - Unknown logger type: %d", type);
            return nullptr;

    }
    return std::static_pointer_cast<ILogger>(logger);
}
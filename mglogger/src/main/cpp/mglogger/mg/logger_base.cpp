//
// Created by sky blue on 2025/7/22.
//

#include "logger_base.h"
#include "logger_hook.h"
#include "logger_fork.h"

using namespace MGLogger;
std::shared_ptr<BaseLogger> BaseLogger::logger = nullptr;

std::shared_ptr<BaseLogger> BaseLogger::CreateLogger(const int type) {
    ALOGD("%s: create %d", __func__, type);
    switch (type) {
        case LOGGER_TYPE_HOOK:
            // 使用 Hook 方式拦截日志
            ALOGD("MGLogger::CreateLogger - Creating LoggerHook");
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
            ALOGD("MGLogger::CreateLogger - Creating LoggerFork");
            if (logger == nullptr) {
                logger = std::make_shared<LoggerFork>();
                if (!logger) {
                    ALOGE("MGLogger::CreateLogger - Failed to create LoggerFork");
                    return nullptr;
                }
            } else {
                ALOGD("MGLogger::CreateLogger - LoggerFork already exists");
            }
            break;
        default:
            ALOGE("MGLogger::CreateLogger - Unknown logger type: %d", type);
            return nullptr;

    }
    return std::static_pointer_cast<BaseLogger>(logger);
}

BaseLogger::BaseLogger() {
    ALOGD("BaseLogger::BaseLogger - initialized");
}

BaseLogger::~BaseLogger() {
    ALOGD("BaseLogger::~BaseLogger - shutting down");
    stop();
}

int BaseLogger::init() {
    ALOGD("BaseLogger::init - initializing logger");
    m_loggerQueue = std::make_shared<LoggerQueue>(500);
    if (!m_loggerQueue) {
        ALOGE("BaseLogger::init - Failed to create LoggerQueue");
        return MG_ERROR;
    } else {
        ALOGD("BaseLogger::init - LoggerQueue created successfully");
    }
    messageQueue = std::make_shared<MessageQueue>();
    if (!messageQueue) {
        ALOGE("BaseLogger::init - Failed to create MessageQueue");
        return MG_ERROR;
    } else {
        ALOGD("BaseLogger::init - MessageQueue created successfully");
    }
    messageQueue->start();
    return MG_OK;
}

void BaseLogger::enqueue(MGLog *log, int tag) {
    if (!m_loggerQueue) {
        ALOGE("BaseLogger::enqueue - LoggerQueue not initialized");
        return;
    }
    m_loggerQueue->enqueue(log, static_cast<LogSourceType>(tag));
}


void BaseLogger::stop() {
    ALOGD("BaseLogger::stop - stopping logger");
    if (m_loggerQueue) {
        m_loggerQueue->abort();
        m_loggerQueue->clear();
    }
    m_loggerQueue = nullptr;
    if (messageQueue) {
        messageQueue->abort();
    }
    messageQueue = nullptr;
    if (!m_blackList.empty()) {
        ALOGD("BaseLogger::stop - clearing blacklist");
        m_blackList.clear();
    }
}





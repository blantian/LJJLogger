#include "logger_base.h"
#include "logger_hook.h"
#include "logger_fork.h"
/**
 * Description: hook/fork 日志采集器的基类实现
 * Created by lantian
 * Date： 2025/7/22
 * Time： 10:20
 */

using namespace MGLogger;

/**
 * 创建日志采集器实例
 * @param type 0 - Hook 方式, 1 - Fork 方式
 * @return
 */

std::shared_ptr<BaseLogger> BaseLogger::CreateLogger(const int type) {
    ALOGD("%s: create %d", __func__, type);
    std::shared_ptr<BaseLogger> logger = nullptr;
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
            // 使用 Fork 方式记录日志
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
    if (m_loggerQueue) {
        m_loggerQueue->clear();
        m_loggerQueue.reset();
    }
    m_loggerQueue = nullptr;
    if (messageQueue) {
        messageQueue.reset();
    }
    messageQueue = nullptr;
    ALOGD("BaseLogger::~BaseLogger - shut down");
}

/**
 * 初始化日志队列，消息队列
 * @return
 */
int BaseLogger::init() {
    ALOGD("BaseLogger::init - initializing logger");
    m_loggerQueue = std::make_shared<LoggerQueue>(500);
    if (!m_loggerQueue) {
        ALOGE("BaseLogger::init - Failed to create LoggerQueue");
        return MG_LOGGER_CREATE_QUEUE_FAILED;
    } else {
        ALOGD("BaseLogger::init - LoggerQueue created successfully");
    }
    messageQueue = std::make_shared<MessageQueue>();
    if (!messageQueue) {
        ALOGE("BaseLogger::init - Failed to create MessageQueue");
        return MG_LOGGER_MESSAGE_QUEUE_FAILED;
    } else {
        ALOGD("BaseLogger::init - MessageQueue created successfully");
    }
    messageQueue->start();
    return MG_OK;
}

/**
 * log入队列
 * @param log
 * @param tag
 */
void BaseLogger::enqueue(MGLog *log, int tag) {
    if (!m_loggerQueue) {
        ALOGE("BaseLogger::enqueue - LoggerQueue not initialized");
        return;
    }
    m_loggerQueue->enqueue(log, static_cast<LogSourceType>(tag));
}

/**
 * 阻塞队列
 */
void BaseLogger::stop() {
    ALOGD("BaseLogger::stop - stopping logger queue and message queue");
    if (m_loggerQueue) {
        m_loggerQueue->abort();
    }
    if (messageQueue) {
        messageQueue->abort();
    }
    ALOGD("BaseLogger::stop - logger queue and message queue stopped");
}





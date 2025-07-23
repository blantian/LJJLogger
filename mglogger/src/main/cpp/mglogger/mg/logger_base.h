//
// Created by sky blue on 2025/7/22.
//

#ifndef MGLOGGER_LOGGER_BASE_H
#define MGLOGGER_LOGGER_BASE_H

#include "ilogger.h"
#include "logger_common.h"
#include "logger_status.h"
#include "vector"

namespace MGLogger {
    /**
     * 基础日志记录器接口
     * 所有日志记录器都应继承自此类
     */
    class BaseLogger : public ILogger {
    public:

        BaseLogger();

        virtual ~BaseLogger();

        // 初始化日志记录器
        int init() override;

        void stop() override;

        int start() override = 0;

        static std::shared_ptr<BaseLogger> CreateLogger(int type);

    protected:

        std::shared_ptr<MGMessage> getMessage() override = 0;

        void writeLog(MGLog *log, int tag) override = 0;

        void enqueue(MGLog *log, int tag) override;

        void setBlackList(const std::list<std::string> &blackList) override  = 0;

        void setLogcatArgs(const std::vector<std::string> &args) override = 0;


        // 工具函数：获取当前线程 ID
        static inline pid_t my_tid() {
#ifdef __ANDROID__
            return gettid();
#else
            return (pid_t) syscall(SYS_gettid);
#endif
        }

        // 工具函数：获取当前时间（毫秒）
        static inline long long getCurrentTimeMillis() {
#if defined(CLOCK_REALTIME)
            struct timespec ts{};
            clock_gettime(CLOCK_REALTIME, &ts);
            return (long long) ts.tv_sec * 1000LL + ts.tv_nsec / 1000000;
#else
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return (long long) tv.tv_sec * 1000LL + tv.tv_usec / 1000;
#endif
        }

        // 工具函数：获取当前进程（主线程）ID
        static inline pid_t my_pid() {
            return getpid();
        }

        bool inline filiterBlackList(MGLog *log) {
            if (!m_blackList.empty() && m_blackList.find(log->tag) != m_blackList.end()) {
                ALOGD("BaseLogger::filterBlackList - Log tag '%s' is in the blacklist, skipping", log->tag);
                return true;
            }
            return false;
        }

    protected:
        static std::shared_ptr<BaseLogger> logger;
        std::shared_ptr<LoggerQueue> m_loggerQueue{nullptr};
        std::shared_ptr<MessageQueue> messageQueue{nullptr};
        std::unordered_set<std::string> m_blackList;  // 黑名单列表
    };
}


#endif //MGLOGGER_LOGGER_BASE_H

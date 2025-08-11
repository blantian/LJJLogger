#ifndef MGLOGGER_LOGGER_FORK_H
#define MGLOGGER_LOGGER_FORK_H

#include "logger_base.h"
#include "logger_common.h"
#include "vector"
#include <atomic>
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/20
 * Time： 14:50
 *
 */

namespace MGLogger {

    class LoggerFork : public BaseLogger {
    public:
        LoggerFork();

        ~LoggerFork() override;

        int init() override;

        void stop() override;

        int start() override;

        int dequeue(MGLog *log) override;

        void setBlackList(const std::list<std::string> &blackList) override;

        void setLogcatArgs(const std::vector<std::string> &args) override;

        std::shared_ptr<MGMessage> getMessage() override;

    private:

        // 写入日志至队列（内部调用 enqueue）
        void writeLog(MGLog *log, int sourceType) override;

        static void parseThreadTimeLine(const char *line, MGLog *log);

        // 启动 Fork 线程
        SDL_Thread *createForkThread();

        // Fork 线程函数
        static int forkThreadFunc(void *arg);

        // 处理 Fork 线程中的日志
        int handleForkLogs();

        inline void sendMessage(int what) {
            messageQueue->sendMessage(what);
        }

        inline void sendMessage(int what, const char *cmd){
            messageQueue->sendMessage(what, cmd);
        }

        inline void sendMessage(const std::shared_ptr<MGMessage> &msg){
            messageQueue->sendMessage(msg);
        }

    private:
        SDL_Thread *forkThread{nullptr};
        SDL_Thread _forkThread{};
        pid_t s_child_pid = -1;
        std::vector<std::string> m_args_str; // logcat 启动参数列表
        std::atomic<bool> s_running{false};
    };

}
#endif //MGLOGGER_LOGGER_FORK_H

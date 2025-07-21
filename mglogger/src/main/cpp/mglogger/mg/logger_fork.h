/**
 * Description:
 * Created by lantian
 * Date： 2025/7/20
 * Time： 14:50
 *
 */

#ifndef MGLOGGER_LOGGER_FORK_H
#define MGLOGGER_LOGGER_FORK_H

#include "ilogger.h"
#include "logger_status.h"

using namespace MGLogger;

class LoggerFork : public ILogger {
public:
    LoggerFork();

    ~LoggerFork();

    int init() override;

    int dequeue(MGLog *log) override;

    // 停止 Fork（中止队列消费）
    void stop() override;

    void setBlackList(const std::list<std::string> &blackList) override;

private:
    // 将日志加入队列
    void enqueue(MGLog *log, int sourceType) override;

    // 写入日志至队列（内部调用 enqueue）
    void writeLog(MGLog *log, int sourceType) override;

    // 启动 Fork 线程
    SDL_Thread *createForkThread();

    // Fork 线程函数
    static int forkThreadFunc(void *arg);

    // 处理 Fork 线程中的日志
    int handleForkLogs();

private:
    static LoggerFork *s_instance;
    SDL_Thread *forkThread{nullptr};
    SDL_Thread _forkThread{};
    pid_t s_child_pid = -1;

};


#endif //MGLOGGER_LOGGER_FORK_H

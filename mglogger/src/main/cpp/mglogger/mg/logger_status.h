//
// Created by sky blue on 2025/7/22.
//

#ifndef MGLOGGER_LOGGER_STATUS_H
#define MGLOGGER_LOGGER_STATUS_H

constexpr const int MG_LOGGER_STATUS_OK = 200;
constexpr const int MG_LOGGER_STATUS_ERROR = 400;
constexpr const int MG_LOGGER_STATUS_INIT_SUCCESS = 210;
constexpr const int MG_LOGGER_STATUS_INIT_FAILED = 410;
constexpr const int MG_LOGGER_STATUS_WRITE = 420;
constexpr const int MG_LOGGER_STATUS_THREAD_CREATION_FAILED = 1001;
constexpr const int MG_LOGGER_STATUS_PIPE_CREATION_FAILED = 1002;
constexpr const int MG_LOGGER_STATUS_PIPE_OPEN_FAILED = 1003;
constexpr const int MG_LOGGER_STATUS_FORK_FAILED = 1004;
constexpr const int MG_LOGGER_STATUS_FORK_EXITED = 1005;
constexpr const int MG_LOGGER_STATUS_FORK_STARTED = 1006;
constexpr const int MG_LOGGER_STATUS_LOGCAT_UNAVAILABLE = 1007;

#endif //MGLOGGER_LOGGER_STATUS_H

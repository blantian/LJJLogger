#ifndef MGLOGGER_LOGGER_FORK_READ_H
#define MGLOGGER_LOGGER_FORK_READ_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*logreader_fail_callback)();

int start_logreader(const char **blacklist, int count, logreader_fail_callback cb);
void stop_logreader();

// Collect logs from log buffers using android_logger_list_read
void collect_log_by_loggerlist();

int hook_log();

#ifdef __cplusplus
}
#endif

#endif //MGLOGGER_LOGGER_FORK_READ_H

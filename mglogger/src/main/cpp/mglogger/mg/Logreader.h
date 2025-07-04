#ifndef MGLOGGER_LOGREADER_H
#define MGLOGGER_LOGREADER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*logreader_fail_callback)();

int start_logreader(const char **blacklist, int count, logreader_fail_callback cb);
void stop_logreader();

// Start collecting logs directly from logd via logdr socket.
// If output_path is nullptr, logs are printed to stdout.
// If pid_filter > 0, only logs from this pid are forwarded.
int start_logdr_reader(const char *output_path, int pid_filter, logreader_fail_callback cb);
void stop_logdr_reader();

#ifdef __cplusplus
}
#endif

#endif //MGLOGGER_LOGREADER_H

#include "Logreader.h"
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cstdint>
#include "android/log.h"

struct logger_entry_v4 {
    uint16_t len;
    uint16_t hdr_size;
    uint32_t pid;
    uint32_t tid;
    uint32_t sec;
    uint32_t nsec;
    uint32_t lid;
    uint32_t uid;
    char msg[0];
};

#define LOG_BUF_SIZE 4096

static bool dr_running = false;
static pthread_t dr_thread;
static logreader_fail_callback dr_fail_cb = nullptr;
static int dr_pid_filter = -1;
static FILE *dr_output = nullptr;

static void *logdr_thread(void *) {
    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        // 打印日志
        __android_log_print(ANDROID_LOG_ERROR, "LogdrReader", "Failed to create socket: %s", strerror(errno));
        if (dr_fail_cb) dr_fail_cb();
        dr_running = false;
        return nullptr;
    }

    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/dev/socket/logdr", sizeof(addr.sun_path) - 1);
    if (connect(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        if (dr_fail_cb) dr_fail_cb();
        close(fd);
        dr_running = false;
        return nullptr;
    }

    const char cmd[] = "stream\0";
    if (write(fd, cmd, sizeof(cmd)) <= 0) {
        if (dr_fail_cb) dr_fail_cb();
        close(fd);
        dr_running = false;
        return nullptr;
    }

    FILE *out = dr_output ? dr_output : stdout;
    char buffer[LOG_BUF_SIZE];
    while (dr_running) {
        ssize_t r = read(fd, buffer, sizeof(buffer));
        if (r <= 0) break;
        size_t off = 0;
        while (off + sizeof(logger_entry_v4) <= (size_t)r) {
            logger_entry_v4 *entry = reinterpret_cast<logger_entry_v4 *>(buffer + off);
            if (off + entry->hdr_size + entry->len > (size_t)r) break;
            if (dr_pid_filter <= 0 || (int)entry->pid == dr_pid_filter) {
                fwrite(entry->msg, 1, entry->len, out);
                fwrite("\n", 1, 1, out);
                fflush(out);
            }
            off += entry->hdr_size + entry->len;
        }
    }
    close(fd);
    if (dr_output && dr_output != stdout) {
        fclose(dr_output);
        dr_output = nullptr;
    }
    dr_running = false;
    return nullptr;
}

int start_logdr_reader(const char *output_path, int pid_filter, logreader_fail_callback cb) {
    if (dr_running) return 0;
    dr_running = true;
    dr_fail_cb = cb;
    dr_pid_filter = pid_filter;
    dr_output = nullptr;
    if (output_path && *output_path) {
        dr_output = fopen(output_path, "w");
        if (!dr_output) {
            dr_running = false;
            if (dr_fail_cb) dr_fail_cb();
            return -1;
        }
    }
    if (pthread_create(&dr_thread, nullptr, logdr_thread, nullptr) != 0) {
        dr_running = false;
        if (dr_output) {
            fclose(dr_output);
            dr_output = nullptr;
        }
        if (dr_fail_cb) dr_fail_cb();
        return -1;
    }
    pthread_detach(dr_thread);
    return 0;
}

void stop_logdr_reader() {
    if (!dr_running) return;
    dr_running = false;
}

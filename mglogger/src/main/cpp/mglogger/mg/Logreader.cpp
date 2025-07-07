#include "Logreader.h"
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <android/log.h>
#include "clogan_core.h"

static bool s_running = false;
static pid_t s_child_pid = -1;
static pthread_t s_thread;
static std::vector<std::string> s_blacklist;
static logreader_fail_callback s_fail_cb = nullptr;

static void *reader_thread(void *) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        if (s_fail_cb) s_fail_cb();
        s_running = false;
        return nullptr;
    }
    s_child_pid = fork();
    if (s_child_pid < 0) {
        if (s_fail_cb) s_fail_cb();
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        s_running = false;
        return nullptr;
    } else if (s_child_pid == 0) {
        // child process
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        dup2(pipe_fd[1], STDERR_FILENO);
        close(pipe_fd[1]);

        std::vector<std::string> args_str;
        args_str.emplace_back("logcat");
        args_str.emplace_back("-v");
        args_str.emplace_back("time");
        for (const auto &tag : s_blacklist) {
            if (!tag.empty()) {
                args_str.emplace_back(tag + ":S");
            }
        }

        std::vector<char *> argv;
        for (auto &s : args_str) {
            argv.push_back(const_cast<char *>(s.c_str()));
        }
        argv.push_back(nullptr);

        execvp("logcat", argv.data());
        _exit(1);
    }
    // parent
    close(pipe_fd[1]);
    FILE *fp = fdopen(pipe_fd[0], "r");
    if (!fp) {
        __android_log_print(ANDROID_LOG_ERROR, "Logreader", "Failed to open pipe for logcat");
        if (s_fail_cb) s_fail_cb();
        close(pipe_fd[0]);
        s_running = false;
        return nullptr;
    }
    char buffer[1024];
    while (s_running && fgets(buffer, sizeof(buffer), fp)) {
        long long ts = (long long)time(nullptr) * 1000LL;
        clogan_write(0, buffer, ts, (char *)"logcat", (long long)gettid(), 0);
    }
    fclose(fp);
    waitpid(s_child_pid, nullptr, 0);
    s_child_pid = -1;
    s_running = false;
    return nullptr;
}

int start_logreader(const char **blacklist, int count, logreader_fail_callback cb) {
    if (s_running) return 0;
    s_running = true;
    s_fail_cb = cb;
    s_blacklist.clear();
    for (int i = 0; i < count; ++i) {
        if (blacklist[i]) {
            s_blacklist.emplace_back(blacklist[i]);
        }
    }
    if (pthread_create(&s_thread, nullptr, reader_thread, nullptr) != 0) {
        s_running = false;
        if (s_fail_cb) s_fail_cb();
        return -1;
    }
    pthread_detach(s_thread);
    return 0;
}

void stop_logreader() {
    if (!s_running) return;
    s_running = false;
    if (s_child_pid > 0) {
        kill(s_child_pid, SIGTERM);
        waitpid(s_child_pid, nullptr, 0);
        s_child_pid = -1;
    }
}

#ifndef MGLOGGER_MG_LOGGER_H
#define MGLOGGER_MG_LOGGER_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>

class logger_queue;
class message_queue;

class mg_logger {
public:
    mg_logger();
    ~mg_logger();
    int init(const char* cache_path, const char* dir_path, int max_file,
             const char* key16, const char* iv16);
    int open(const char* file_name);
    int write(int flag, const char* log, long long local_time,
              const char* thread_name, long long thread_id, int is_main);
    int flush();
    void debug(int debug);
    void stop();
    void registerCallback(void (*cb)(int, const char*));
private:
    logger_queue* m_queue;
    message_queue* m_statusQueue;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

int mg_logger_init(const char* cache_path, const char* dir_path, int max_file,
                   const char* key16, const char* iv16);
int mg_logger_open(const char* file_name);
int mg_logger_write(int flag, const char* log, long long local_time,
                    const char* thread_name, long long thread_id, int is_main);
int mg_logger_flush();
void mg_logger_debug(int debug);
void mg_logger_release();
void mg_logger_register_callback(void (*cb)(int, const char*));

#ifdef __cplusplus
}
#endif

#endif // MGLOGGER_MG_LOGGER_H

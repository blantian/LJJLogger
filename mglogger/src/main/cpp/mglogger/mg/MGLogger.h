/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 14:50
 */


#ifndef MGLOGGER_MGLOGGER_H
#define MGLOGGER_MGLOGGER_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>

class message_queue;

class MGLogger {
public:
    MGLogger();
    ~MGLogger();

    int init(const char* cache_path, const char* dir_path, int max_file,
              const char* key16, const char* iv16);
    int open(const char* file_name);
    int write(int flag, const char* log, long long local_time,
              const char* thread_name, long long thread_id, int is_main);
    int flush();
    void debug(int debug);
    void stop();

private:
    message_queue* m_queue;
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

#ifdef __cplusplus
}
#endif

#endif //MGLOGGER_MGLOGGER_H

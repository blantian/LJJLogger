/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 14:50
 */


#include "mg_logger.h"
#include "logger_queue.h"
#include "mglogger/logan/clogan_core.h"

mg_logger::mg_logger() {
    m_queue = new logger_queue();
}

mg_logger::~mg_logger() {
    stop();
    delete m_queue;
    m_queue = nullptr;
}

int mg_logger::init(const char* cache_path, const char* dir_path, int max_file,
                    const char* key16, const char* iv16) {
    return m_queue->dispatch_sync([=]() {
        return clogan_init(cache_path, dir_path, max_file, key16, iv16);
    });
}

int mg_logger::open(const char* file_name) {
    return m_queue->dispatch_sync([=]() {
        return clogan_open(file_name);
    });
}

int mg_logger::write(int flag, const char* log, long long local_time,
                     const char* thread_name, long long thread_id, int is_main) {
    int ret = m_queue->dispatch([=]() {
        clogan_write(flag, (char*)log, local_time, (char*)thread_name,
                     thread_id, is_main);
    });
    return ret;
}

int mg_logger::flush() {
    return m_queue->dispatch_sync([=]() { return clogan_flush(); });
}

void mg_logger::debug(int debug) {
    m_queue->dispatch([=]() { clogan_debug(debug); });
}

void mg_logger::stop() {
    if (m_queue) {
        m_queue->stop();
    }
}

static mg_logger g_logger;

extern "C" {

int mg_logger_init(const char* cache_path, const char* dir_path, int max_file,
                   const char* key16, const char* iv16) {
    return g_logger.init(cache_path, dir_path, max_file, key16, iv16);
}

int mg_logger_open(const char* file_name) {
    return g_logger.open(file_name);
}

int mg_logger_write(int flag, const char* log, long long local_time,
                    const char* thread_name, long long thread_id, int is_main) {
    return g_logger.write(flag, log, local_time, thread_name, thread_id, is_main);
}

int mg_logger_flush() {
    return g_logger.flush();
}

void mg_logger_debug(int debug) {
    g_logger.debug(debug);
}

void mg_logger_release() {
    g_logger.stop();
}

} // extern "C"


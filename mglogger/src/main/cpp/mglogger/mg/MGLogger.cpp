/**
 * Description:
 * Created by lantian 
 * Date： 2025/7/11
 * Time： 14:50
 */


#include "MGLogger.h"
#include "message_queue.h"
#include "mglogger/logan/clogan_core.h"

MGLogger::MGLogger() {
    m_queue = new message_queue();
}

MGLogger::~MGLogger() {
    stop();
    delete m_queue;
    m_queue = nullptr;
}

int MGLogger::init(const char* cache_path, const char* dir_path, int max_file,
                   const char* key16, const char* iv16) {
    return m_queue->dispatch([=]() {
        return clogan_init(cache_path, dir_path, max_file, key16, iv16);
    });
}

int MGLogger::open(const char* file_name) {
    return m_queue->dispatch([=]() {
        return clogan_open(file_name);
    });
}

int MGLogger::write(int flag, const char* log, long long local_time,
                    const char* thread_name, long long thread_id, int is_main) {
    return m_queue->dispatch([=]() {
        return clogan_write(flag, (char*)log, local_time, (char*)thread_name,
                            thread_id, is_main);
    });
}

int MGLogger::flush() {
    return m_queue->dispatch([=]() { return clogan_flush(); });
}

void MGLogger::debug(int debug) {
    m_queue->dispatch_void([=]() { clogan_debug(debug); });
}

void MGLogger::stop() {
    if (m_queue) {
        m_queue->stop();
    }
}

static MGLogger g_logger;

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


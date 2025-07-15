/**
 * Description:
 * Created by lantian
 * Date： 2025/7/11
 * Time： 14:50
 */


#include "mg_logger.h"
#include "logger_queue.h"
#include "message_queue.h"
#include "mglogger/logan/clogan_core.h"  // CLogan API header
#include "clogan_status.h"

static mg_logger g_logger;

mg_logger::mg_logger() {
    m_queue = new logger_queue();
    m_statusQueue = new message_queue();
}

mg_logger::~mg_logger() {
    stop();
    delete m_queue;
    m_queue = nullptr;
    delete m_statusQueue;
    m_statusQueue = nullptr;
}

int mg_logger::init(const char* cache_path, const char* dir_path, int max_file,
                    const char* key16, const char* iv16) {
    std::string cachePathStr(cache_path ? cache_path : "");
    std::string dirPathStr(dir_path ? dir_path : "");
    std::string keyStr(key16 ? key16 : "");
    std::string ivStr(iv16 ? iv16 : "");
    m_queue->enqueue([=]() {
        int code =  clogan_init(cachePathStr.c_str(), dirPathStr.c_str(), max_file,
                    keyStr.c_str(), ivStr.c_str());
        if (m_statusQueue) {
            m_statusQueue->push(code, CLGOAN_INIT_STATUS);
        }
        return code;
    });

    return 0;
}

int mg_logger::open(const char* file_name) {
    std::string fileNameStr(file_name ? file_name : "");
    m_queue->enqueue([=]() {
        clogan_open(fileNameStr.c_str());
    });
    return 0;
}

int mg_logger::write(int flag, const char* log, long long local_time,
                     const char* thread_name, long long thread_id, int is_main) {
    std::string logStr(log ? log : "");
    std::string threadNameStr(thread_name ? thread_name : "");
    m_queue->enqueue([=]() {
        clogan_write(flag,
                     const_cast<char*>(logStr.c_str()),
                     local_time,
                     const_cast<char*>(threadNameStr.c_str()),
                     thread_id,
                     is_main);
    });
    return 0;
}

int mg_logger::flush() {
    m_queue->enqueue([=]() {
        clogan_flush();
    });
    return 0;
}

void mg_logger::debug(int debug) {
    m_queue->enqueue([=]() {
        clogan_debug(debug);
    });
}

void mg_logger::stop() {
    if (m_queue) {
        m_queue->stop();
    }
}

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



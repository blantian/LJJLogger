/**
 * Description:
 * Created by lantian 
 * Date： 2025/6/30
 * Time： 19:04
 */


#include <jni.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include "NativeLogReaderJni.h"

static std::string g_system_log;

static void read_system_log(int max_lines) {
    std::string cmd = "logcat -d -t " + std::to_string(max_lines);
    FILE *pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        return;
    }
    char buffer[1024];
    std::stringstream ss;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        ss << buffer;
    }
    pclose(pipe);
    g_system_log = ss.str();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mgtv_logger_kt_log_MGLoggerJni_nativeGetSystemLog(
        JNIEnv *env,
        jobject /* thiz */,
        jint max_lines) {
    g_system_log.clear();
    read_system_log(max_lines);
    return env->NewStringUTF(g_system_log.c_str());
}

//
// Created by sky blue on 2025/7/24.
//

#ifndef MGLOGGER_FILE_UTILS_H
#define MGLOGGER_FILE_UTILS_H
#include "map"
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#ifdef __cplusplus
}
#endif

class FileUtils {
public:
    /**
     * 获取指定目录下的文件信息
     * @param path 文件或目录路径
     * @return 存在返回 true，否则返回 false
     */

    static std::map<std::string, long long> collectFileInfo(const char* path);

};


#endif //MGLOGGER_FILE_UTILS_H

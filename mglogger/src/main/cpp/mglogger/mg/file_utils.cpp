//
// Created by sky blue on 2025/7/24.
//

#include "file_utils.h"

std::map<std::string, long long> FileUtils::collectFileInfo(const char* path) {
    std::map<std::string, long long> result;

    if (!path || *path == '\0') {
        ALOGD("path is null or empty");
        return result;
    }

    DIR* dir = opendir(path);
    if (!dir) {
        ALOGD("opendir failed: %s", path);
        return result;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        // 跳过 .  和  ..
        if (entry->d_name[0] == '.') continue;

        // 拼完整路径
        char fullPath[PATH_MAX];
        std::snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // stat 判断是否为普通文件并获取大小
        struct stat st{};
        if (stat(fullPath, &st) != 0 || !S_ISREG(st.st_mode))
            continue;
        result[entry->d_name] = static_cast<long long>(st.st_size);
    }
    closedir(dir);
    return result;
}
//
// Created by sky blue on 2025/7/24.
//

#pragma once
#include <cstdint>
#include "string"
#include <cerrno>
#include <stdexcept>
#include <limits>
#include "map"
#include <dirent.h>
#include <sys/stat.h>
#include <chrono>
#include <cstdio>
#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#ifdef __cplusplus
}
#endif

namespace utils {

    class LoggerUtils {
    public:
        LoggerUtils() = delete;

        // uint64_t -> const char*
        // 使用 thread_local 避免返回的指针在函数返回后失效
        static inline const char* toCString(uint64_t value) {
            thread_local char buf[32];
            std::snprintf(buf, sizeof(buf), "%llu",
                          static_cast<unsigned long long>(value));
            return buf;
        }

        // uint64_t -> std::string
        static inline std::string toString(uint64_t value) {
            // 20 位足够 (2^64-1 = 18446744073709551615, 字符数20)
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%llu",
                          static_cast<unsigned long long>(value));
            return {buf};
        }

        static inline uint64_t toUint64(const char* str) {
            if (!str) {
                throw std::invalid_argument("toUint64: null string");
            }
            errno = 0;
            char* end = nullptr;
            unsigned long long v = std::strtoull(str, &end, 10);
            if (errno != 0 || end == str) {
                throw std::invalid_argument("toUint64: invalid numeric string");
            }
            return static_cast<uint64_t>(v);
        }

        // e.g. "1234567890.log" -> 1234567890
        static inline uint64_t parseTsFromFileName(const std::string& filename) {
            // 找最后一个 '.'，截断扩展名
            auto dot = filename.find_last_of('.');
            std::string numeric = (dot == std::string::npos) ? filename : filename.substr(0, dot);
            return toUint64(numeric.c_str());
        }


        static inline std::map<std::string, long long> collectFileInfo(const char* path) {
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

        // Merge all log cache files in the given directory into a single
        // output file. Files are concatenated in ascending order of file name.
        // Returns true on success and false if no files were merged or an
        // error occurred.
        static inline bool mergeCacheFiles(const char* dir_path,
                                           const char* out_path) {
            if (!dir_path || !out_path) {
                ALOGD("mergeCacheFiles: invalid param");
                return false;
            }

            auto infos = collectFileInfo(dir_path);
            if (infos.empty()) {
                return false;
            }

            FILE* out = std::fopen(out_path, "wb");
            if (!out) {
                ALOGD("mergeCacheFiles: failed to open output %s", out_path);
                return false;
            }

            bool merged = false;
            char buffer[4096];
            char fullPath[PATH_MAX];

            for (const auto& kv : infos) {
                std::snprintf(fullPath, sizeof(fullPath), "%s/%s",
                              dir_path, kv.first.c_str());
                FILE* in = std::fopen(fullPath, "rb");
                if (!in) {
                    ALOGD("mergeCacheFiles: failed to open input %s", fullPath);
                    continue;
                }
                size_t n;
                while ((n = std::fread(buffer, 1, sizeof(buffer), in)) > 0) {
                    if (std::fwrite(buffer, 1, n, out) != n) {
                        std::fclose(in);
                        std::fclose(out);
                        ALOGD("mergeCacheFiles: write failed");
                        return false;
                    }
                }
                std::fclose(in);
                merged = true;
            }

            std::fclose(out);
            if (!merged) {
                std::remove(out_path);
            }
            return merged;
        }

        static inline uint64_t nowMs() {
            using namespace std::chrono;
            return duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch()
            ).count();          // 单位：毫秒
        }
    };
}

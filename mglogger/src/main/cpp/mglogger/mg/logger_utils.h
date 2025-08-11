
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
#include <cstdint>
#include <cstdio>
#include <zlib.h>
#include <arpa/inet.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "sdl_log.h"
#ifdef __cplusplus
}
#endif

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/24
 * Time： 16:01
 *
 */
namespace utils {

    class LoggerUtils {
    public:
        LoggerUtils() = delete;

        /**
         * uint64_t -> const char*
         * 使用 thread_local 避免返回的指针在函数返回后失效
         */
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

        /**
         * 收集指定目录下的所有普通文件信息
         * @param path 目录路径
         * @return 文件名和大小的映射
         */
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

        static inline int mergeCompressedFiles(const char* dirPath, const char* outName) {
            if (!dirPath || !outName) {
                ALOGE("mergeCompressedFiles - Invalid arguments");
                return MG_ERROR;
            }

            auto files = collectFileInfo(dirPath);
            if (files.empty()) {
                ALOGE("mergeCompressedFiles - No files found in directory: %s", dirPath);
                return -2;
            }

            // 创建输出文件
            FILE* outFile = std::fopen(outName, "wb");
            if (!outFile) {
                ALOGE("mergeCompressedFiles - Failed to create output file: %s", outName);
                return -3;
            }

            int status = 0;
            for (const auto &kv : files) {
                std::string fullPath;
                if (dirPath[strlen(dirPath)-1] == '/') {
                    fullPath = std::string(dirPath) + kv.first;
                } else {
                    fullPath = std::string(dirPath) + "/" + kv.first;
                }

                FILE* inFile = std::fopen(fullPath.c_str(), "rb");
                if (!inFile) {
                    ALOGE("mergeCompressedFiles - Failed to open file: %s", fullPath.c_str());
                    status = -4;
                    break;
                }

                // 写入文件头信息（使用网络字节序保证兼容性）
                uint32_t nameLen = htonl(static_cast<uint32_t>(kv.first.size()));
                uint64_t fileLen = htonll_android(static_cast<uint64_t>(kv.second));

                if (std::fwrite(&nameLen, sizeof(nameLen), 1, outFile) != 1 ||
                    std::fwrite(kv.first.data(), ntohl(nameLen), 1, outFile) != 1 ||
                    std::fwrite(&fileLen, sizeof(fileLen), 1, outFile) != 1) {
                    ALOGE("mergeCompressedFiles - Failed to write file header for: %s", kv.first.c_str());
                    status = -5;
                    std::fclose(inFile);
                    break;
                }

                // 直接复制已压缩的文件内容
                char buffer[64 * 1024];  // 增大缓冲区提高性能
                size_t bytesRead;
                while ((bytesRead = std::fread(buffer, 1, sizeof(buffer), inFile)) > 0) {
                    if (std::fwrite(buffer, 1, bytesRead, outFile) != bytesRead) {
                        ALOGE("mergeCompressedFiles - Failed to write file data for: %s", kv.first.c_str());
                        status = -6;
                        break;
                    }
                }

                // 检查读取错误
                if (ferror(inFile)) {
                    ALOGE("mergeCompressedFiles - File read error: %s", fullPath.c_str());
                    status = -7;
                }

                std::fclose(inFile);
                if (status != 0) {
                    break;
                }
            }

            std::fclose(outFile);
            return status;
        }

        // Android NDK 可能没有 htonll，需要自实现
        static inline uint64_t htonll_android(uint64_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl(value >> 32);
#else
            return value;
#endif
        }

        static inline uint64_t nowMs() {
            using namespace std::chrono;
            return duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch()
            ).count();          // 单位：毫秒
        }
    };
}

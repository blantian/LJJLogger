package com.mgtv.logger.kt.log

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:23
 */
data class LoggerConfig(
    val cachePath: String,
    val logDir: String,
    val keepDays: Long,
    val maxFile: Long,
    val minSdCard: Long,
    val maxQueue: Int,
    val key16: String,
    val iv16: String
) {
    class Builder {
        var cachePath: String = ""
        var logDir: String = ""
        var keepDays: Long = 1 * 24 * 60 * 60 * 1000L
        var maxFile: Long = 10 * 1024 * 1024
        var minSdCard: Long = 50 * 1024 * 1024
        var maxQueue: Int = 10_000
        var key16: String = "1234567890abcdef"
        var iv16: String = "abcdef1234567890"
        fun putCachePath(path: String): Builder = apply { cachePath = path }
        fun putLogDir(dir: String): Builder = apply { logDir = dir }
        fun putKeepDays(days: Long): Builder = apply { keepDays = days }
        fun putMaxFile(value: Long): Builder = apply { maxFile = value }
        fun putMinSdCard(value: Long): Builder = apply { minSdCard = value }
        fun putMaxQueue(value: Int): Builder = apply { maxQueue = value }
        fun putKey16(key: String): Builder = apply { this.key16 = key }
        fun putIv16(iv: String): Builder = apply { this.iv16 = iv }
        fun build() =
            LoggerConfig(cachePath, logDir, keepDays, maxFile, minSdCard, maxQueue, key16, iv16)
    }
}

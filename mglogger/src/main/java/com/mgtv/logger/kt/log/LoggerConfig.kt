package com.mgtv.logger.kt.log

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:23
 */
public data class LoggerConfig(
    public val cachePath: String,
    public val logDir: String,
    public val keepDays: Long,
    public val maxFile: Long,
    public val minSdCard: Long,
    public val maxQueue: Int,
    public val key16: String,
    public val iv16: String
) {
    public class Builder {
        public var cachePath: String = ""
        public var logDir: String = ""
        public var keepDays: Long = 1 * 24 * 60 * 60 * 1000L
        public var maxFile: Long = 10 * 1024 * 1024
        public var minSdCard: Long = 50 * 1024 * 1024
        public var maxQueue: Int = 10_000
        public var key16: String = "1234567890abcdef"
        public var iv16: String = "abcdef1234567890"
        public fun putCachePath(path: String): Builder = apply { cachePath = path }
        public fun putLogDir(dir: String): Builder = apply { logDir = dir }
        public fun putKeepDays(days: Long): Builder = apply { keepDays = days }
        public fun putMaxFile(value: Long): Builder = apply { maxFile = value }
        public fun putMinSdCard(value: Long): Builder = apply { minSdCard = value }
        public fun putMaxQueue(value: Int): Builder = apply { maxQueue = value }
        public fun putKey16(key: String): Builder = apply { this.key16 = key }
        public fun putIv16(iv: String): Builder = apply { this.iv16 = iv }
        public fun build() =
            LoggerConfig(cachePath, logDir, keepDays, maxFile, minSdCard, maxQueue, key16, iv16)
    }
}

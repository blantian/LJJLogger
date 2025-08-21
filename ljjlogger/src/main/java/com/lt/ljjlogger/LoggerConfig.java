package com.lt.ljjlogger;

import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

/**
 * Description:
 * Created by lantian
 * Date： 2025/8/1
 * Time： 15:20
 */
public final class LoggerConfig {

    /* ---------- 必填 ---------- */
    private final String cachePath;
    private final String logDir;

    /* ---------- 可选 ---------- */
    private final int logCacheS;
    private final long maxLogFileBytes;
    private final long maxSdCardBytes;
    private final int maxQueue;
    private final String key16;
    private final String iv16;
    private final List<String> logcatBlackList;   // 不可变

    /* =================== 构造器私有 =================== */
    private LoggerConfig(Builder b) {
        this.cachePath = b.cachePath;
        this.logDir = b.logDir;
        this.logCacheS = b.logCacheS;
        this.maxLogFileBytes = b.maxLogFileBytes;
        this.maxSdCardBytes = b.maxSdCardBytes;
        this.maxQueue = b.maxQueue;
        this.key16 = b.key16;
        this.iv16 = b.iv16;
        this.logcatBlackList = Collections.unmodifiableList(new ArrayList<>(b.logcatBlackList));
    }

    /* =================== Getter =================== */
    @NonNull
    public String getCachePath() {
        return cachePath;
    }

    @NonNull
    public String getLogDir() {
        return logDir;
    }

    public int getLogCacheS() {
        return logCacheS;
    }

    public long getMaxLogFileBytes() {
        return maxLogFileBytes;
    }

    public long getMaxSdCardBytes() {
        return maxSdCardBytes;
    }

    public int getMaxQueue() {
        return maxQueue;
    }

    @NonNull
    public String getKey16() {
        return key16;
    }

    @NonNull
    public String getIv16() {
        return iv16;
    }

    @NonNull
    public List<String> getLogcatBlackList() {
        return logcatBlackList;
    }

    /* =================== Builder =================== */
    public static Builder builder(@NonNull String cachePath, @NonNull String logDir) {
        return new Builder(cachePath, logDir);
    }

    boolean isValid() {
        return !TextUtils.isEmpty(cachePath) && !TextUtils.isEmpty(logDir) && key16 != null
                && iv16 != null;
    }

    /**
     * 基于现有配置派生修改
     */
    public Builder toBuilder() {
        return new Builder(this);
    }

    public static final class Builder {
        private final String cachePath;
        private final String logDir;

        /* 可选 (默认值) */
        private int logCacheS = Defaults.LOG_CACHE_S;
        private long maxLogFileBytes = Defaults.MAX_FILE;
        private long maxSdCardBytes = Defaults.MIN_SD_CARD;
        private int maxQueue = Defaults.MAX_QUEUE;
        private String key16 = Defaults.KEY16;
        private String iv16 = Defaults.IV16;
        private List<String> logcatBlackList = Defaults.LOGCAT_BLACK;

        private Builder(String cachePath, String logDir) {
            this.cachePath = Objects.requireNonNull(cachePath, "cachePath == null");
            this.logDir = Objects.requireNonNull(logDir, "logDir == null");
        }

        private Builder(LoggerConfig src) {
            this.cachePath = src.cachePath;
            this.logDir = src.logDir;
            this.logCacheS = src.logCacheS;
            this.maxLogFileBytes = src.maxLogFileBytes;
            this.maxSdCardBytes = src.maxSdCardBytes;
            this.maxQueue = src.maxQueue;
            this.key16 = src.key16;
            this.iv16 = src.iv16;
            this.logcatBlackList = new ArrayList<>(src.logcatBlackList);
        }

        public Builder nativeLogCacheSelector(int selector /* 0:hook 1:logcat */) {
            this.logCacheS = selector;
            return this;
        }

        public Builder maxLogFileBytes(long bytes) {
            this.maxLogFileBytes = bytes;
            return this;
        }

        public Builder maxSdCardBytes(long bytes) {
            this.maxSdCardBytes = bytes;
            return this;
        }

        public Builder maxQueueSize(int size) {
            this.maxQueue = size;
            return this;
        }

        public Builder key16(@NonNull String key) {
            this.key16 = key;
            return this;
        }

        public Builder iv16(@NonNull String iv) {
            this.iv16 = iv;
            return this;
        }

        public Builder logcatBlackList(@Nullable List<String> list) {
            List<String> merged = new ArrayList<>(Defaults.LOGCAT_BLACK);
            if (list != null) {
                merged.addAll(list);
            }
            logcatBlackList = merged;
            return this;
        }

        /* ---------- 构建 ---------- */
        public LoggerConfig build() {
            // 可以在此添加更多参数合法性检查
            return new LoggerConfig(this);
        }
    }

    /* =================== 默认常量集中管理 =================== */
    public static final class Defaults {
        public static final int LOG_CACHE_S = 0;
        public static final long MAX_FILE = 1024 * 1024;           // 1 MiB
        public static final long MIN_SD_CARD = 10L * 1024 * 1024;         // 10 MiB
        public static final int MAX_QUEUE = 500;                       // 队列最大长度
        public static final String KEY16 = "1234567890abcdef";
        public static final String IV16 = "abcdef1234567890";
        public static final List<String> LOGCAT_BLACK;

        static {
            List<String> list = new ArrayList<>();
            list.add("MGLogger");
            list.add("LoggerNativeBridge");
            list.add("LoggerControlCenter");
            list.add("LoggerConfig");
            list.add("LoggerWorker");
            LOGCAT_BLACK = Collections.unmodifiableList(list);
        }

        private Defaults() {
        }
    }

    @NonNull
    @Override
    public String toString() {
        return "LoggerConfig{" +
                "cachePath='" + cachePath + '\'' +
                ", logDir='" + logDir + '\'' +
                ", logCacheS=" + logCacheS +
                ", maxFile=" + maxLogFileBytes +
                ", minSdCard=" + maxSdCardBytes +
                ", maxQueue=" + maxQueue +
                ", key16='" + key16 + '\'' +
                ", iv16='" + iv16 + '\'' +
                ", logcatBlackList=" + logcatBlackList +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof LoggerConfig)) return false;
        LoggerConfig that = (LoggerConfig) o;
        return logCacheS == that.logCacheS &&
                maxLogFileBytes == that.maxLogFileBytes &&
                maxSdCardBytes == that.maxSdCardBytes &&
                maxQueue == that.maxQueue &&
                cachePath.equals(that.cachePath) &&
                logDir.equals(that.logDir) &&
                key16.equals(that.key16) &&
                iv16.equals(that.iv16) &&
                logcatBlackList.equals(that.logcatBlackList);
    }

    @Override
    public int hashCode() {
        return Objects.hash(cachePath, logDir, logCacheS, maxLogFileBytes,
                maxSdCardBytes, maxQueue, key16, iv16, logcatBlackList);
    }
}

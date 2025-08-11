package com.mgtv.logger;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.mgtv.logger.i.ILogger;
import com.mgtv.logger.i.ILoggerStatus;

import java.util.List;
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/31
 * Time： 10:25
 */
public class MGLoggerProtocol implements ILogger {

    private static final String TAG = "MGLogger";
    static boolean sDebug = false;

    private volatile boolean initialised;

    private static volatile MGLoggerProtocol instance;

    @NonNull
    private ILogger delegate = new NoOpProtocol();

    @Nullable
    private ILoggerStatus statusListener;

    private MGLoggerProtocol() {}

    public static MGLoggerProtocol get() {
        if (instance == null) {
            synchronized (MGLoggerProtocol.class) {
                if (instance == null) {
                    instance = new MGLoggerProtocol();
                }
            }
        }
        return instance;
    }

    @Override
    public void loggerInit(String cachePath,
                           String dirPath,
                           int logCacheSelector,
                           int maxFile,
                           int maxSdCardSize,
                           String encryptKey16,
                           String encryptIv16) {
        if (initialised) return;

        if (LoggerNativeBridge.isLoggerOk()) {
            LoggerNativeBridge real = LoggerNativeBridge.getInstance();
            real.setOnLoggerStatus(statusListener);
            real.loggerInit(cachePath, dirPath, logCacheSelector,maxFile,maxSdCardSize, encryptKey16, encryptIv16);
            delegate = real;
            initialised = true;
        } else {
            delegate = new NoOpProtocol();
        }
    }

    @Override
    public void loggerWrite(int flag, String log, long localTime, String threadName, long threadId, boolean isMain) {
        ensureInitialised();
        if (sDebug) {
            Log.d(TAG, "loggerWrite called with flag=" + flag + ", log=" + log +
                    ", localTime=" + localTime + ", threadName=" + threadName +
                    ", threadId=" + threadId + ", isMain=" + isMain);
        }
        delegate.loggerWrite(flag, log, localTime, threadName, threadId, isMain);
    }

    @Override
    public void loggerOpen(String fileName) {
        ensureInitialised();
        if (sDebug) {
            Log.d(TAG, "loggerOpen called with fileName=" + fileName);
        }
        delegate.loggerOpen(fileName);
    }

    @Override
    public void loggerFlush() {
        ensureInitialised();
        if (sDebug) {
            Log.d(TAG, "loggerFlush called.");
        }
        delegate.loggerFlush();
    }

    @Override
    public void loggerDebug(boolean debug) {
        sDebug = debug;
        if (sDebug) {
            Log.d(TAG, "loggerDebug set to " + debug);
        }
        ensureInitialised();
        delegate.loggerDebug(debug);
    }

    @Override
    public void setBlackList(List<String> blackList) {
        ensureInitialised();
        if (sDebug) {
            Log.d(TAG, "setBlackList called with " + blackList);
        }
        delegate.setBlackList(blackList);
    }

    @Override
    public int exportLog(String fileName) {
        Log.i(TAG, "exportLog called with fileName=" + fileName);
        ensureInitialised();
        if (sDebug) {
            Log.d(TAG, "exportLog called fileName=" + fileName);
        }
        return delegate.exportLog(fileName);
    }

    @Override
    public void setOnLoggerStatus(ILoggerStatus listener) {
        this.statusListener = listener;
        if (sDebug) {
            Log.d(TAG, "setOnLoggerStatus called with listener=" + listener);
        }
    }

    public boolean isInitialised() {
        return initialised;
    }


    private void ensureInitialised() {
        if (!initialised) {
            throw new IllegalStateException("MGLogger not initialised. Call init() first.");
        }
    }


    /* ********************* no‑op fallback ********************** */

    private static final class NoOpProtocol implements ILogger {

        @Override
        public void loggerInit(String cachePath, String dirPath, int logCacheSelector, int maxFile, int maxSdCardSize, String encryptKey16, String encryptIv16) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: loggerInit called, but this is a no-op implementation.");
            }
        }

        @Override
        public void loggerWrite(int flag, String log, long localTime, String threadName, long threadId, boolean isMain) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: loggerWrite called with flag=" + flag + ", log=" + log +
                        ", localTime=" + localTime + ", threadName=" + threadName +
                        ", threadId=" + threadId + ", isMain=" + isMain);
            }
        }

        @Override
        public void loggerOpen(String fileName) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: loggerOpen called with fileName=" + fileName);
            }
        }

        @Override
        public void loggerFlush() {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: loggerFlush called.");
            }
        }

        @Override
        public void loggerDebug(boolean debug) {
            sDebug = debug;
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: loggerDebug set to " + debug);
            }
        }

        @Override
        public void setBlackList(List<String> blackList) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: setBlackList called with " + blackList);
            }
        }

        @Override
        public int exportLog(String fileName) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: exportLog called with fileName=" + fileName);
            }
            return -1;
        }

        @Override
        public void setOnLoggerStatus(ILoggerStatus listener) {
            if (sDebug) {
                Log.d(TAG, "NoOpProtocol: setOnLoggerStatus called with listener=" + listener);
            }
        }
    }

}

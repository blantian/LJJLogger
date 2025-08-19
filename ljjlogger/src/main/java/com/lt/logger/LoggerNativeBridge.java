package com.lt.logger;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import androidx.annotation.Keep;

import com.lt.logger.i.ILogger;
import com.lt.logger.i.ILoggerStatus;

import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Description:
 * Created by lantian
 * Date： 2025/8/1
 * Time： 11:23
 */
public class LoggerNativeBridge implements ILogger {

    private static final String TAG = "LoggerNativeBridge";
    private static volatile LoggerNativeBridge sInstance;
    private static boolean isMGLoggerOk;

    /**
     * logger 是否 init
     */
    private boolean isLoganInit = false;
    /**
     * logger 是否 open
     */
    private boolean isLoganOpen = false;

    private ILoggerStatus loggerStatus;

    private final Handler handler = new EventHandler(Looper.getMainLooper());

    /**
     * 记录已上报的错误码，避免重复回调
     */
    private final Set<Integer> errorSet =
            Collections.synchronizedSet(new HashSet<>());

    static {
        try {
            System.loadLibrary("ljjlogger");
            isMGLoggerOk = true;
        } catch (Throwable t) {
            Log.e(TAG, "Failed to load logger library", t);
            isMGLoggerOk = false;
        }
    }

    private static class EventHandler extends Handler {
        public EventHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            // 处理消息
            Log.i(TAG, "EventHandler received message: " + msg.what);
        }
    }


    public static LoggerNativeBridge getInstance() {
        if (sInstance == null) {
            synchronized (LoggerNativeBridge.class) {
                if (sInstance == null) {
                    sInstance = new LoggerNativeBridge();
                }
            }
        }
        return sInstance;
    }


    static boolean isLoggerOk() {
        return isMGLoggerOk;
    }

    private native int initLogger(String cachePath,
                                  String dirPath,
                                  int logCacheSelector,
                                  int maxFile,
                                  int maxSdCardSize,
                                  String encryptKey16,
                                  String encryptIv16);


    private native int open4Logger(String fileName);

    private native void debug4Logger(boolean isDebug);

    private native int write4Logger(int flag,
                                    String log,
                                    long localTime,
                                    String threadName,
                                    long threadId,
                                    int isMain);

    private native void flush4Logger();

    private native void LoggerSetBlackList(String[] list);

    private native int exportLog4Logger(String fileName);


    @Override
    public void setOnLoggerStatus(ILoggerStatus listener) {
        this.loggerStatus = listener;
    }

    @Override
    public void setBlackList(List<String> blackList) {
        Log.d(TAG, "setBlackList called with size: " + blackList.size());
        if (!isMGLoggerOk) return;
        try {
            LoggerSetBlackList(blackList.toArray(new String[0]));
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to set black list", e);
        }
    }

    @Override
    public int exportLog(String fileName) {
        Log.i(TAG, "exportLog called with fileName: " + fileName);
        if (!isMGLoggerOk) {
            return -1;
        }
        return exportLog4Logger(fileName);
    }


    @Override
    public void loggerInit(String cachePath,
                           String dirPath,
                           int logCacheSelector,
                           int maxFile,
                           int maxSdCardSize,
                           String encryptKey16,
                           String encryptIv16) {

        if (isLoganInit) return;

        if (!isMGLoggerOk) {
            loggerStatusCode(LJJLoggerStatus.MGLOGGER_LOAD_SO,
                    LJJLoggerStatus.MGLOGGER_LOAD_SO_FAIL);
            return;
        }

        try {
            Log.i(TAG, "logger_init called with cachePath: "
                    + cachePath + ", dirPath: " + dirPath
                    + ", logCacheSelector: " + logCacheSelector
                    + ", maxFile: " + maxFile
                    + ", maxSdCardSize: " + maxSdCardSize
                    + ", encryptKey16: " + encryptKey16
                    + ", encryptIv16: " + encryptIv16);

            int code = initLogger(cachePath, dirPath,
                    logCacheSelector, maxFile, maxSdCardSize,
                    encryptKey16, encryptIv16);
            isLoganInit = true;
            handler.post(() -> {
                // 初始化完成后清理错误记录，允许重新上报
                handleLoggerStatus(code, LJJLoggerStatus.MGLOGGER_INIT_STATUS);
            });
//            loggerStatusCode(MGLoggerStatus.MGLOGGER_INIT_STATUS, code);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to initialize logger", e);
            loggerStatusCode(LJJLoggerStatus.MGLOGGER_INIT_STATUS,
                    LJJLoggerStatus.MGLOGGER_INIT_FAIL_JNI);
        }
    }


    @Override
    public void loggerWrite(int flag,
                            String log,
                            long localTime,
                            String threadName,
                            long threadId,
                            boolean isMain) {

        if (!isLoganOpen || !isMGLoggerOk) return;

        try {
            int code = write4Logger(flag, log, localTime,
                    threadName, threadId, isMain ? 1 : 0);

            if (code != LJJLoggerStatus.MGLOGGER_WRITE_SUCCESS
                    || LJJLoggerProtocol.sDebug) {
                loggerStatusCode(LJJLoggerStatus.MGLOGGER_WRITE_STATUS, code);
            }
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to write log", e);
            loggerStatusCode(LJJLoggerStatus.MGLOGGER_WRITE_STATUS,
                    LJJLoggerStatus.MGLOGGER_WRITE_FAIL_JNI);
        }
    }

    @Override
    public void loggerOpen(String fileName) {
        if (!isLoganInit || !isMGLoggerOk) return;

        try {
            int code = open4Logger(fileName);
            isLoganOpen = true;
            loggerStatusCode(LJJLoggerStatus.MGLOGGER_OPEN_STATUS, code);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to open logger", e);
            loggerStatusCode(LJJLoggerStatus.MGLOGGER_OPEN_STATUS,
                    LJJLoggerStatus.MGLOGGER_OPEN_FAIL_JNI);
        }
    }


    @Override
    public void loggerFlush() {
        try {
            Log.i(TAG, "loggerFlush called");
            flush4Logger();
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to flush logger", e);
        }
    }

    @Override
    public void loggerDebug(boolean debug) {
        if (!isLoganInit || !isMGLoggerOk) return;
        try {
            debug4Logger(debug);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to set logger debug mode", e);
        }
    }

    @Keep
    public void onLoggerStatus(int code, String cmd) {
        Log.i(TAG, "onLoggerStatus cmd: " + cmd + ", code: " + code);
        handler.post(() -> handleLoggerStatus(code, cmd));
    }

    private void handleLoggerStatus(int code, String cmd) {
        loggerStatusCode(cmd, code);
    }

    private void loggerStatusCode(String cmd, int code) {
        Log.i(TAG, "loggerStatusCode cmd: " + cmd + ", code: " + code);
        boolean isWriteCmd = cmd.endsWith(LJJLoggerStatus.MGLOGGER_WRITE_STATUS);
        if (isWriteCmd
                && code != LJJLoggerStatus.MGLOGGER_INIT_FAIL_JNI
                && !errorSet.add(code)) {
            return; // 已经回调过，避免重复
        }
        if (code == LJJLoggerStatus.MGLOGGER_OK && cmd.equals(LJJLoggerStatus.MGLOGGER_INIT_STATUS)) {
            // 初始化成功
        } else if (code == LJJLoggerStatus.MGLOGGER_OK && cmd.equals(LJJLoggerStatus.MGLOGGER_OPEN_STATUS)) {
        } else if (code != LJJLoggerStatus.MGLOGGER_OK) {
            Log.e(TAG, "Logger error: " + cmd + " with code: " + code);
        }
        if (loggerStatus != null) {
            loggerStatus.loggerStatus(code, cmd);
        }
    }

}

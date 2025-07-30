package com.mgtv.logger.log;

import android.util.Log;

import androidx.annotation.Keep;

import com.mgtv.logger.log.i.ILogger;
import com.mgtv.logger.log.i.ILoggerStatus;

import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class LoggerNativeBridge implements ILogger {

    private static final String TAG = "LoggerNativeBridge";
    private static volatile LoggerNativeBridge sInstance;
    private static boolean isMGLoggerOk;

    /** logger 是否 init */
    private boolean isLoganInit = false;
    /** logger 是否 open */
    private boolean isLoganOpen = false;

    private ILoggerStatus loggerStatus;

    /** 记录已上报的错误码，避免重复回调 */
    private final Set<Integer> errorSet =
            Collections.synchronizedSet(new HashSet<>());

    static {
        try {
            System.loadLibrary("mglogger");
            isMGLoggerOk = true;
        } catch (Throwable t) {
            Log.e(TAG, "Failed to load logger library", t);
            isMGLoggerOk = false;
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

    public static native int initLogger( String cachePath,
                                          String dirPath,
                                          int    logCacheSelector,
                                          int    maxFile,
                                          int    maxSdCardSize,
                                          String encryptKey16,
                                          String encryptIv16);


    private native int  LoggerOpen(String fileName);
    private native void LoggerDebug(boolean isDebug);
    private native int  LoggerWrite(int flag,
                                    String log,
                                    long   localTime,
                                    String threadName,
                                    long   threadId,
                                    int    isMain);
    private native void LoggerFlush();
    private native void LoggerSetBlackList(String[] list);


    @Override
    public void setOnLoggerStatus(ILoggerStatus listener) {
        this.loggerStatus = listener;
    }

    @Override
    public void setBlackList(List<String> blackList) {
        if (!isMGLoggerOk) return;
        try {
            LoggerSetBlackList(blackList.toArray(new String[0]));
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to set black list", e);
        }
    }


    @Override
    public void loggerInit(String cachePath,
                            String dirPath,
                            int    logCacheSelector,
                            int    maxFile,
                            int    maxSdCardSize,
                            String encryptKey16,
                            String encryptIv16) {

        if (isLoganInit) return;

        if (!isMGLoggerOk) {
            loggerStatusCode(MGLoggerStatus.MGLOGGER_LOAD_SO,
                    MGLoggerStatus.MGLOGGER_LOAD_SO_FAIL);
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
            loggerStatusCode(MGLoggerStatus.MGLOGGER_INIT_STATUS, code);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to initialize logger", e);
            loggerStatusCode(MGLoggerStatus.MGLOGGER_INIT_STATUS,
                    MGLoggerStatus.MGLOGGER_INIT_FAIL_JNI);
        }
    }


    @Override
    public void loggerWrite( int    flag,
                             String log,
                             long   localTime,
                             String threadName,
                             long   threadId,
                             boolean isMain) {

        if (!isLoganOpen || !isMGLoggerOk) return;

        try {
            int code = LoggerWrite(flag, log, localTime,
                    threadName, threadId, isMain ? 1 : 0);

            if (code != MGLoggerStatus.MGLOGGER_WRITE_SUCCESS
                    || MGLoggerProtocol.sDebug) {
                loggerStatusCode(MGLoggerStatus.MGLOGGER_WRITE_STATUS, code);
            }
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to write log", e);
            loggerStatusCode(MGLoggerStatus.MGLOGGER_WRITE_STATUS,
                    MGLoggerStatus.MGLOGGER_WRITE_FAIL_JNI);
        }
    }

    @Override
    public void loggerOpen(String fileName) {
        if (!isLoganInit || !isMGLoggerOk) return;

        try {
            int code = LoggerOpen(fileName);
            isLoganOpen = true;
            loggerStatusCode(MGLoggerStatus.MGLOGGER_OPEN_STATUS, code);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to open logger", e);
            loggerStatusCode(MGLoggerStatus.MGLOGGER_OPEN_STATUS,
                    MGLoggerStatus.MGLOGGER_OPEN_FAIL_JNI);
        }
    }


    @Override
    public void loggerFlush() {
        try {
            Log.i(TAG, "loggerFlush called");
            LoggerFlush();
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to flush logger", e);
        }
    }

    @Override
    public void loggerDebug(boolean debug) {
        if (!isLoganInit || !isMGLoggerOk) return;
        try {
            LoggerDebug(debug);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to set logger debug mode", e);
        }
    }

    @Keep
    public void onLoggerStatus(int code, String cmd) {
        loggerStatusCode(cmd, code);
    }


    private void loggerStatusCode(String cmd, int code) {
        Log.i(TAG, "loggerStatusCode cmd: " + cmd + ", code: " + code);
        boolean isWriteCmd = cmd.endsWith(MGLoggerStatus.MGLOGGER_WRITE_STATUS);
        if (isWriteCmd
                && code != MGLoggerStatus.MGLOGGER_INIT_FAIL_JNI
                && !errorSet.add(code)) {
            return; // 已经回调过，避免重复
        }
        if (loggerStatus != null) {
            loggerStatus.loggerStatus(code, cmd);
        }
    }

}

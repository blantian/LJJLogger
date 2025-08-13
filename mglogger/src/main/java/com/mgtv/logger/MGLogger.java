package com.mgtv.logger;


import android.content.Context;
import android.util.Log;

import com.mgtv.logger.i.ILoggerStatus;

import java.io.File;
import java.util.HashMap;
import java.util.Map;
/**
 * Description:
 * Created by lantian
 * Date： 2025/8/1
 * Time： 14:51
 */
public class MGLogger {
    private static final String TAG = "MGLogger";
    private static ILoggerStatus statusListener;
    private static LoggerControlCenter loggerControlCenter;
    private static boolean isHookOrFork = false;
    static boolean sDebug = false;

    public static void init(LoggerConfig config) {
        loggerControlCenter = LoggerControlCenter.getInstance(config);
        loggerControlCenter.start();
    }

    public static void write(String log, int type) {
        // 已经hook或fork的情况，不用打点
        if (isHookOrFork) {
            return;
        }

        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize MGLogger first");
        }
        loggerControlCenter.write(log, type);
    }

    public static void flush() {
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize MGLogger first");
        }
        loggerControlCenter.flush();
    }

    /**
     * 发送日志到服务器
     * @param sendLogRunnable 发送日志的Runnable
     */
    public static void sendLog(Context context,SendLogRunnable sendLogRunnable) {
        Log.i(TAG, "sendLog");
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize MGLogger first");
        }
        loggerControlCenter.send(context,sendLogRunnable);
    }

    /**
     * 获取所有日志文件的信息
     * @return 包含文件名和大小的Map，文件名为日期字符串，大小为文件长度
     */
    public static Map<String, Long> getAllFilesInfo() {
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize Logan first");
        }
        File dir = loggerControlCenter.getDir();
        if (!dir.exists()) {
            return null;
        }
        File[] files = dir.listFiles();
        if (files == null) {
            return null;
        }
        Map<String, Long> allFilesInfo = new HashMap<>();
        for (File file : files) {
            try {
                allFilesInfo.put(LoggerUtils.getDateStr(Long.parseLong(file.getName())), file.length());
            } catch (NumberFormatException e) {
                // ignore
            }
        }
        return allFilesInfo;
    }

    public static void setDebug(boolean debug) {
        sDebug = debug;
    }

    static void onListenerLogWriteStatus(String name, int status) {
        if (name.equals(MGLoggerStatus.MGLOGGER_INIT_STATUS)) {
            isHookOrFork = status == MGLoggerStatus.MGLOGGER_OK;
        }

        if (statusListener != null) {
            statusListener.loggerStatus(status,name);
        }
    }

    public static void setStatusListener(ILoggerStatus listener) {
        statusListener = listener;
    }

}

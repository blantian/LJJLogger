package com.mgtv.logger.log;

import android.util.Log;

import com.mgtv.logger.log.i.ILoggerStatus;
import com.mgtv.logger.log.i.SendLogCallback;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

public class MGLogger {
    private static final String TAG = "MGLogger";
    private static ILoggerStatus statusListener;
    private static LoggerControlCenter loggerControlCenter;

    static boolean sDebug = false;

    public static void init(LoggerConfig config) {
        loggerControlCenter = LoggerControlCenter.getInstance(config);
//        CrashHandler.install();
    }


    public static void start() {
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize MGLogger first");
        }
        loggerControlCenter.start();
    }

    public static void write(String log, int type) {
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

    public static void sendLog(String[] dates, SendLogRunnable sendLogRunnable) {
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize MGLogger first");
        }
        loggerControlCenter.send(dates, sendLogRunnable);
    }

    /**
     * @param url             接受日志的服务器完整url.
     * @param date            日志日期 格式："2018-11-21".
     * @param appId           当前应用的唯一标识,在多App时区分日志来源App.
     * @param unionId         当前用户的唯一标识,用来区分日志来源用户.
     * @param deviceId        设备id.
     * @param buildVersion    上报源App的build号.
     * @param appVersion      上报源的App版本.
     * @param sendLogCallback 上报结果回调（子线程调用）.
     */
    public static void send(String url, String date, String appId, String unionId, String deviceId,
                         String buildVersion, String appVersion, SendLogCallback sendLogCallback) {
        final Map<String, String> headers = new HashMap<>();
        headers.put("fileDate", date);
        headers.put("appId", appId);
        headers.put("unionId", unionId);
        headers.put("deviceId", deviceId);
        headers.put("buildVersion", buildVersion);
        headers.put("appVersion", appVersion);
        headers.put("platform", "1");
        send(url, date, headers, sendLogCallback);
    }

    /**
     * @param url             接受日志的服务器完整url.
     * @param date            日志日期 格式："2018-11-21".
     * @param headers         请求头信息.
     * @param sendLogCallback 上报结果回调（子线程调用）.
     */
    public static void send(String url, String date, Map<String, String> headers, SendLogCallback sendLogCallback) {
        if (loggerControlCenter == null) {
            throw new RuntimeException("Please initialize Logan first");
        }
        final SendLogDefaultRunnable sendLogRunnable = new SendLogDefaultRunnable();
        sendLogRunnable.setUrl(url);
        sendLogRunnable.setSendLogCallback(sendLogCallback);
        sendLogRunnable.setRequestHeader(headers);
        loggerControlCenter.send(new String[]{date}, sendLogRunnable);
    }


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
                allFilesInfo.put(Util.getDateStr(Long.parseLong(file.getName())), file.length());
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
        if (name.equals(MGLoggerStatus.MGLOGGER_INIT_STATUS) && status != MGLoggerStatus.MGLOGGER_OK) {
            //todo 处理初始化失败的情况，可切换java层收集log模式
            Log.d(TAG, "MGLogger init failed with status: " + status);
        }

        if (statusListener != null) {
            statusListener.loggerStatus(status,name);
        }
    }

    public static void setStatusListener(ILoggerStatus listener) {
        statusListener = listener;
    }

}

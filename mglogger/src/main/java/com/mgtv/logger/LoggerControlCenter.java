package com.mgtv.logger;

import android.content.Context;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Description:
 * Created by lantian
 * Date： 2025/8/2
 * Time： 10:40
 * 日志调度中枢。
 * <p>
 * 负责把外部写入、发送、flush 等请求转为 {@link LoggerModel} 入队，
 * 再交由后台线程统一处理。
 */
public final class LoggerControlCenter {

    private static volatile LoggerControlCenter sInstance;

    public static LoggerControlCenter getInstance(LoggerConfig config) {
        if (sInstance == null) {
            synchronized (LoggerControlCenter.class) {
                if (sInstance == null) {
                    sInstance = new LoggerControlCenter(config);
                }
            }
        }
        return sInstance;
    }

    /* **********   成员变量   ********** */
    private final BlockingQueue<LoggerModel> logQueue;
    private final String logPath;
    private final LoggerWorker loggerWorker;

    /**
     * 线程安全的日期解析器
     */
    private static final ThreadLocal<SimpleDateFormat> DATE_FORMATTER = new ThreadLocal<SimpleDateFormat>() {
        @Override
        protected SimpleDateFormat initialValue() {
            return new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault());
        }
    };

    /* **********   构造   ********** */
    private LoggerControlCenter(LoggerConfig config) {
        if (config == null || !config.isValid()) {
            throw new IllegalArgumentException("LoganConfig is null or invalid.");
        }

        this.logPath = config.getLogDir();
        String cachePath = config.getCachePath();
        long maxSdCardSpace = config.getMaxSdCardBytes();
        long maxLogFileSize = config.getMaxLogFileBytes();
        int logCacheSelector = config.getLogCacheS();
        long maxQueueSize = config.getMaxQueue();
        String encryptKey16 = new String(config.getKey16().getBytes());
        String encryptIv16 = new String(config.getIv16().getBytes());
        List<String> blackList = config.getLogcatBlackList();

        this.logQueue = new LinkedBlockingQueue<>((int) maxQueueSize);

        this.loggerWorker = new LoggerWorker(
                logQueue,
                cachePath,
                logPath,
                logCacheSelector,
                maxLogFileSize,
                maxSdCardSpace,
                encryptKey16,
                encryptIv16,
                blackList
        );
    }


    public void start() {
        if (loggerWorker != null) {
            loggerWorker.start();
        }
    }

    /* **********   对外 API   ********** */

    /**
     * 写日志
     */
    public void write(String log, int flag) {
        if (TextUtils.isEmpty(log)) return;

        LoggerModel model = new LoggerModel();
        model.action = LoggerModel.Action.WRITE;

        WriteAction action = new WriteAction();
        action.log = log;
        action.localTime = System.currentTimeMillis();
        action.flag = flag;
        action.isMainThread = Looper.getMainLooper() == Looper.myLooper();
        action.threadId = Thread.currentThread().getId();
        action.threadName = Thread.currentThread().getName();
        model.writeAction = action;

        enqueue(model);
    }

    /**
     * 发送日志
     */
    public void send(Context context, SendLogRunnable runnable) {
        String logsPath = LoggerUtils.getExternalCacheDirPath(context);
        if (TextUtils.isEmpty(logsPath)) return;
        LoggerModel model = new LoggerModel();
        model.action = LoggerModel.Action.SEND;
        SendAction action = new SendAction();
        action.sendLogRunnable = runnable;
        action.uploadPath = logsPath + "/log" + System.currentTimeMillis();
        model.sendAction = action;
        enqueue(model);
    }

    /**
     * 强制 flush
     */
    public void flush() {
        if (TextUtils.isEmpty(logPath)) return;

        LoggerModel model = new LoggerModel();
        model.action = LoggerModel.Action.FLUSH;
        enqueue(model);
    }

    /**
     * 获取日志目录
     */
    public File getDir() {
        return new File(logPath);
    }

    /* **********   内部工具方法   ********** */

    private void enqueue(LoggerModel model) {
        // offer() 不阻塞，队列满时直接丢弃，后期据业务改为阻塞 put()
        if (logQueue.offer(model)) {
            Log.i("LoggerControlCenter", "Log added to queue: " + model.action);
            // 队列添加成功，表示有空间，可以继续处理
        } else {
            Log.w("LoggerControlCenter", "Log queue is full, log discarded: " + model.action);
            // 队列已满，当前日志被丢弃
            // 可在此处做降级处理，例如写入本地文件或上报统计
        }
    }

    private long parseDateMillis(String date) {
        try {
            if (TextUtils.isEmpty(date)) {
                return -1L;
            }
            // 使用线程安全的 SimpleDateFormat 解析日期字符串
            SimpleDateFormat formatter = DATE_FORMATTER.get();
            if (formatter == null) {
                DATE_FORMATTER.set(new SimpleDateFormat("yyyy-MM-dd", Locale.getDefault()));
            }

            if (formatter == null) {
                return -1L; // 解析器初始化失败
            }

            Date dateIn = formatter.parse(date);

            if (dateIn == null) {
                return -1L; // 解析失败
            }

            return dateIn.getTime();
        } catch (ParseException e) {
            return -1L;
        }
    }
}

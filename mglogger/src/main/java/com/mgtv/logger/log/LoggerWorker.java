package com.mgtv.logger.log;

import android.os.StatFs;
import android.text.TextUtils;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.nio.file.StandardOpenOption;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 后台日志处理线程。
 * 负责 WRITE / FLUSH / SEND 三种动作。
 */
final class LoggerWorker implements Runnable {

    private static final String TAG = "LoggerWorker";
    private static final int MINUTE = 60 * 1000;
    private static final int CACHE_SIZE = 1024;


    private final BlockingQueue<LoggerModel> queue;
    private final String cachePath;
    private final String logPath;
    private final long maxLogFile;
    private final long maxSdCardBytes;
    private final int logCacheSelector;
    private final String encryptKey16;
    private final String encryptIv16;
    private final List<String> logcatBlackList;
    private final long saveTimeMs = 7 * 24 * 60 * 60 * 1000L; // 默认保存7天

    /** 发送中缓存 */
    private final ConcurrentLinkedQueue<LoggerModel> pendingSendQueue = new ConcurrentLinkedQueue<>();
    private final Object sendLock = new Object();
    private volatile int sendStatus = SendLogRunnable.IDLE;

    private final ExecutorService sendExecutor = Executors.newSingleThreadExecutor(r -> {
        Thread t = new Thread(r, "logger-send-worker");
        t.setPriority(Thread.NORM_PRIORITY);
        return t;
    });

    private final AtomicBoolean running = new AtomicBoolean(true);
    private Thread workerThread;

    private MGLoggerProtocol protocol;
    private long currentDay;
    private boolean sdWritable = true;
    private long lastCheckTime;
    private File logDir;

    LoggerWorker(BlockingQueue<LoggerModel> queue,
                 String cachePath,
                 String logPath,
                 int logCacheSelector,
                 long maxLogFile,
                 long minSdCardBytes,
                 String encryptKey16,
                 String encryptIv16,
                 List<String> logcatBlackList) {
        this.queue = Objects.requireNonNull(queue);
        this.cachePath = cachePath;
        this.logPath = logPath;
        this.logCacheSelector = logCacheSelector;
        this.maxLogFile = maxLogFile;
        this.maxSdCardBytes = minSdCardBytes;
        this.encryptKey16 = encryptKey16;
        this.encryptIv16 = encryptIv16;
        this.logcatBlackList = logcatBlackList;
        ensureProtocolInit();
    }

    /** 启动线程 */
    void start() {
        if (workerThread != null) return;
        workerThread = new Thread(this, "logger-worker");
        workerThread.start();
    }


    /** 退出 */
    void quit() {
        running.set(false);
        if (workerThread != null) {
            workerThread.interrupt();
        }
    }

    @Override
    public void run() {
        while (running.get()) {
            try {
                LoggerModel model = queue.take(); // 阻塞
                handle(model);
            } catch (InterruptedException e) {
                // 如果是主动 quit，跳出；其他情况继续
                if (!running.get()) break;
            } catch (Throwable t) {
                Log.e(TAG, "LoggerWorker loop error", t);
            }
        }
        // 退出前 flush 一下
        flushInternal();
    }

    /* **************** core **************** */

    private void handle(LoggerModel model) {
        if (model == null || !model.isValid()) return;

        ensureProtocolInit();

        switch (model.action) {
            case WRITE:
                doWrite(model.writeAction);
                break;
            case FLUSH:
                flushInternal();
                break;
            case SEND:
                if (model.sendAction != null && model.sendAction.isValid()) {
                    doSend(model);
                }
                break;
        }
    }

    private void ensureProtocolInit() {
        if (protocol != null) return;
        protocol = MGLoggerProtocol.get();
        protocol.setOnLoggerStatus((cmd, code) ->
                MGLogger.onListenerLogWriteStatus(code, cmd));
        protocol.loggerInit(cachePath, logPath, logCacheSelector,(int) maxLogFile, (int) maxSdCardBytes, encryptKey16, encryptIv16);
        protocol.setBlackList(logcatBlackList);
        protocol.loggerDebug(MGLogger.sDebug);
    }

    private void doWrite(WriteAction action) {
        Log.i(TAG, "doWrite");
        if (action == null) return;

        if (logDir == null) logDir = new File(logPath);

        rotateAndCleanupIfNeeded();

        // 每分钟检测一次空间
        long now = System.currentTimeMillis();
        if (now - lastCheckTime > MINUTE) {
            sdWritable = checkSdWritable();
            lastCheckTime = now;
        }
        if (!sdWritable) return;

        protocol.loggerWrite(action.flag, action.log, action.localTime,
                action.threadName, action.threadId, action.isMainThread);
    }

    private void rotateAndCleanupIfNeeded() {
        long nowDay = Util.getCurrentTime();
        if (currentDay >= nowDay && currentDay + 24L * 60 * 60 * 1000 > System.currentTimeMillis()) {
            return;
        }
        long deleteTime = nowDay - saveTimeMs;
        deleteExpiredFiles(deleteTime);
        currentDay = nowDay;
        protocol.loggerOpen(String.valueOf(currentDay));
    }

    private void flushInternal() {
        Log.i(TAG, "Flushing logs to disk...");
        if (protocol != null) {
            Log.i(TAG, "Flushing logs to disk...");
            protocol.loggerFlush();
        }
    }

    private void doSend(LoggerModel action) {
        synchronized (sendLock) {
            if (sendStatus == SendLogRunnable.STATUS_SENDING) {
                pendingSendQueue.add(action);
                return;
            }
            if (!prepareUploadFile(action.sendAction)) return;

            sendStatus = SendLogRunnable.STATUS_SENDING;
            action.sendAction.sendLogRunnable.setSendAction(action.sendAction);
            action.sendAction.sendLogRunnable.setCallBackListener(statusCode -> {
                synchronized (sendLock) {
                    sendStatus = statusCode;
                    if (statusCode == SendLogRunnable.STATUS_FINISHED) {
                        // 把排队的 SEND 全部回灌
                        LoggerModel m;
                        while ((m = pendingSendQueue.poll()) != null) {
                            if (queue.offer(m)){
                                Log.d(TAG, "Re-queued pending send action: " + m.sendAction.date);
                            } else {
                                Log.e(TAG, "Failed to re-queue pending send action: " + m.sendAction.date);
                            }
                        }
                    }
                }
            });
            sendExecutor.execute(action.sendAction.sendLogRunnable);
        }
    }

    private boolean prepareUploadFile(SendAction action) {
        if (TextUtils.isEmpty(logPath)) return false;

        if (!exists(action.date)) {
            action.uploadPath = "";
            return false;
        }

        String src = logPath + File.separator + action.date;
        if (action.date.equals(String.valueOf(Util.getCurrentTime()))) {
            flushInternal();
            String des = src + ".copy";
            if (copyFileFast(src, des)) {
                action.uploadPath = des;
                return true;
            }
            return false;
        } else {
            action.uploadPath = src;
            return true;
        }
    }

    /* **************** utils **************** */

    private boolean copyFileFast(String src, String des) {
        try (FileInputStream fis = new FileInputStream(src); FileOutputStream fos = new FileOutputStream(des)) {
            byte[] buffer = new byte[CACHE_SIZE];
            int bytesRead;
            while ((bytesRead = fis.read(buffer)) != -1) {
                fos.write(buffer, 0, bytesRead);
            }
            return true;
        } catch (IOException e) {
            Log.e(TAG, "copyFileFast error", e);
            return false;
        }
    }

    private boolean checkSdWritable() {
        boolean item = false;
        try {
            StatFs stat = new StatFs(logPath);
            long blockSize = stat.getBlockSize();
            long availableBlocks = stat.getAvailableBlocks();
            long total = availableBlocks * blockSize;
            if (total > maxSdCardBytes) { //判断SDK卡
                item = true;
            }
        } catch (IllegalArgumentException e) {
            Log.e(TAG, "checkSdWritable error", e);
        }
        return item;
    }

    private boolean exists(String name) {
        if (TextUtils.isEmpty(logPath)) return false;
        File f = new File(logPath, name);
        return f.exists() && f.isFile();
    }

    private void deleteExpiredFiles(long deleteTime) {
        File dir = new File(logPath);
        if (!dir.isDirectory()) return;

        String[] files = dir.list();
        if (files == null) return;

        for (String item : files) {
            try {
                if (TextUtils.isEmpty(item)) continue;
                String[] arr = item.split("\\.");
                if (arr.length > 0) {
                    long ts = Long.parseLong(arr[0]);
                    if (ts <= deleteTime && arr.length == 1) {
                        new File(logPath, item).delete();
                    }
                }
            } catch (Throwable ignore) { }
        }
    }
}

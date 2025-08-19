package com.lt.mglogger.log.utils;

import android.util.Log;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class LogPrinter {
    private static final String TAG = "LogPrinter";
    private static final long DEFAULT_INTERVAL_MS = 10;

    private static volatile LogPrinter sInstance;

    public static LogPrinter get() {
        if (sInstance == null) {
            synchronized (LogPrinter.class) {
                if (sInstance == null) {
                    sInstance = new LogPrinter();
                }
            }
        }
        return sInstance;
    }

    private LogPrinter() { /* no-op */ }


    private final ScheduledExecutorService scheduler =
            Executors.newSingleThreadScheduledExecutor(r -> {
                Thread t = new Thread(r, "LogPrinterPrinterThread");
                t.setDaemon(true); // 避免阻止进程退出
                return t;
            });

    /** 当前任务句柄，用于取消 */
    private ScheduledFuture<?> future;

    /* ---------------- 对外 API ---------------- */

    public static void start() {
        get().startInternal("Hi, I'm LogPrinter", DEFAULT_INTERVAL_MS);
    }

    /**
     * 自定义消息与间隔
     *
     * @param msg        打印的字符串
     * @param intervalMs 间隔（毫秒）
     */
    public static void start(String msg, long intervalMs) {
        get().startInternal(msg, intervalMs);
    }

    /** 结束打印 */
    public static void stop() {
        get().stopInternal();
    }

    /** 彻底销毁（通常不需要调用） */
    public static void destroy() {
        get().destroyInternal();
    }

    /* ---------------- 内部逻辑 ---------------- */

    private synchronized void startInternal(String msg, long intervalMs) {
        // 已经在跑就不重复启动
        if (future != null && !future.isCancelled()) return;

        future = scheduler.scheduleAtFixedRate(
                () -> Log.d(TAG, msg),
                0,
                intervalMs,
                TimeUnit.MILLISECONDS
        );
    }

    private synchronized void stopInternal() {
        if (future != null) {
            future.cancel(true);
            future = null;
        }
    }

    private synchronized void destroyInternal() {
        stopInternal();
        scheduler.shutdownNow();
    }
}

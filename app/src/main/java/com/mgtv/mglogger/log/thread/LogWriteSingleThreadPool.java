package com.mgtv.mglogger.log.thread;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * global thread
 */
public class LogWriteSingleThreadPool {

    private static final int SIZE = 1;
    private static final int MAX_SIZE = 2;
    private static final int QUEUE_CAPACITY = 128;
    private static ThreadPoolExecutor mPool;

    public static ThreadPoolExecutor getInstance() {
        if (mPool == null) {
            synchronized (LogWriteSingleThreadPool.class) {
                if (mPool == null) {
                    mPool = new ThreadPoolExecutor(SIZE, MAX_SIZE, 3, TimeUnit.SECONDS,
                            new LinkedBlockingQueue<Runnable>(QUEUE_CAPACITY), new CustomThreadFactory(),
                            new ThreadPoolExecutor.CallerRunsPolicy());
                }
            }
        }
        return mPool;
    }

    /**
     * run a thead ,== new thread
     */
    public static void startRunInThread(Runnable doSthRunnable) {
        getInstance().execute(doSthRunnable);
    }

    /**
     * shut down
     */
    public static void shutdownPool() {
        if (mPool != null) {
            mPool.shutdown();
            mPool = null;
        }
    }
}

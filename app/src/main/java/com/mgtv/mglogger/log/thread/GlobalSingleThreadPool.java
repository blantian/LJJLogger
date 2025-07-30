package com.mgtv.mglogger.log.thread;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * global thread
 */
public class GlobalSingleThreadPool {

    private static final int SIZE = 1;
    private static final int MAX_SIZE = 2;
    private static ThreadPoolExecutor mPool;

    public static ThreadPoolExecutor getGlobalSingleThreadPoolInstance() {
        if (mPool == null) {
            synchronized (GlobalSingleThreadPool.class) {
                if (mPool == null) {
                    mPool = new ThreadPoolExecutor(SIZE, MAX_SIZE, 3,
                            TimeUnit.SECONDS,
                            new LinkedBlockingQueue<Runnable>());
                }
            }
        }
        return mPool;
    }

    /**
     * run a thead ,== new thread
     */
    public static void startRunInSingleThreadPool(Runnable doSthRunnable) {
        getGlobalSingleThreadPoolInstance().execute(doSthRunnable);
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

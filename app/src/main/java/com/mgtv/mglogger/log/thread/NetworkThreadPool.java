package com.mgtv.mglogger.log.thread;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class NetworkThreadPool {
    private static final int SIZE = 3;
    private static final int MAX_SIZE = 5;
    private static ThreadPoolExecutor mPool;

     public static ThreadPoolExecutor getGlobalThreadPoolInstance() {
        if (mPool == null) {
            synchronized (NetworkThreadPool.class) {
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
    public static void startRunInThread(Runnable doSthRunnable) {
        getGlobalThreadPoolInstance().execute(doSthRunnable);
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

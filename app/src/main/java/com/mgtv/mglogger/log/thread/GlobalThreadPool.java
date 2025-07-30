package com.mgtv.mglogger.log.thread;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * global thread
 */
public class GlobalThreadPool {

    private static final int SIZE = 3;
    private static final int MAX_SIZE = 5;
    private static ThreadPoolExecutor mPool;
    //不限制个数的线程池
    private static ExecutorService mUnLimitedPool = Executors.newCachedThreadPool();

    public static ThreadPoolExecutor getGlobalThreadPoolInstance() {
        if (mPool == null) {
            synchronized (GlobalThreadPool.class) {
                if (mPool == null) {
                    mPool = new ThreadPoolExecutor(SIZE, MAX_SIZE, 3,
                            TimeUnit.SECONDS,
                            new LinkedBlockingQueue<Runnable>());
                }
            }
        }
        return mPool;
    }

    public static ExecutorService getUnLimitedThreadPool() {
        return mUnLimitedPool;
    }

    /**
     * run a thead ,== new thread
     */
    public static void startRunInThread(Runnable doSthRunnable) {
        getGlobalThreadPoolInstance().execute(doSthRunnable);
    }

    public static void startSingleThread(Runnable runnable) {
        new Thread(runnable).start();
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

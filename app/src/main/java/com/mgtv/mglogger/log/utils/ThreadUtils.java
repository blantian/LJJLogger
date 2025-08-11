package com.mgtv.mglogger.log.utils;

import android.os.Looper;

import com.mgtv.mglogger.log.thread.LogWriteSingleThreadPool;

/**
 * 开启线程
 */
public class ThreadUtils {

    /**
     * 启动写日志线程
     * @param doSthRunnable
     */
    public static void startRunLogWriteSingleThread(Runnable doSthRunnable) {
        LogWriteSingleThreadPool.startRunInThread(doSthRunnable);
    }

    /**
     * 关闭线程池
     */
    public static void shutdownThreadPool(){
        LogWriteSingleThreadPool.shutdownPool();
    }

    /**
     * 判断是否在主线程执行
     */
    public static boolean isInMainThread() {
        return Looper.myLooper() == Looper.getMainLooper();
    }
}

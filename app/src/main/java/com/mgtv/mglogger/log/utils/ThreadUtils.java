package com.mgtv.mglogger.log.utils;

import android.os.Looper;

import com.mgtv.mglogger.log.thread.GlobalJsonThreadPool;
import com.mgtv.mglogger.log.thread.GlobalSingleThreadPool;
import com.mgtv.mglogger.log.thread.LogWriteSingleThreadPool;
import com.mgtv.mglogger.log.thread.NetworkThreadPool;
import com.mgtv.mglogger.log.thread.ReportJsonThreadPool;

/**
 * 开启线程
 */
public class ThreadUtils {

    /**
     * start run sth in thread pool
     * @param doSthRunnable
     */
    public static void startRunInThread(Runnable doSthRunnable) {
        GlobalJsonThreadPool.startRunInThread(doSthRunnable);
    }

    /**
     * start run report sth in thread pool
     * @param doSthRunnable
     */
    public static void startRunInReportThread(Runnable doSthRunnable){
        ReportJsonThreadPool.startRunInThread(doSthRunnable);
    }

    public static void startRunInSinleThreadPool(Runnable doSthRunnable) {
        GlobalSingleThreadPool.startRunInSingleThreadPool(doSthRunnable);
    }

    public static void startRunInSingleThread(Runnable runnable) {
        new Thread(runnable).start();
    }

    public static void startRunInThreadForClearQueue(Runnable doSthRunnable) {
//        GlobalSingleThread.getInstance(ContextProvider.getApplicationContext())
//                .startRunAndClearQueue(doSthRunnable);
    }

    /**
     * start run network task in thread pool
     * @param doSthRunnable
     */
    public static void startRunInNetworkThread(Runnable doSthRunnable){
        NetworkThreadPool.startRunInThread(doSthRunnable);
    }

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
        GlobalJsonThreadPool.shutdownPool();
        GlobalSingleThreadPool.shutdownPool();
        ReportJsonThreadPool.shutdownPool();
    }

    /**
     * 判断是否在主线程执行
     */
    public static boolean isInMainThread() {
        return Looper.myLooper() == Looper.getMainLooper();
    }
}

package com.mgtv.mglogger.log.thread;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

/**
 * global single thread
 */
public class GlobalSingleThread extends Handler {

    private static final String THREAD_NAME = "GlobalSingleThread";
    private static final int MSG_GET_TJ_DATA = 0;
    private static final int MSG_GET_PLAY_DATA = 1;
    private static HandlerThread mGlobalHandlerThread;
    private static GlobalSingleThread mGlobalSingleThread;

    private GlobalSingleThread(Context context, Looper looper) {
        super(looper);
    }

    public static GlobalSingleThread getInstance(Context context) {
        if (mGlobalHandlerThread == null) {
            mGlobalHandlerThread = new HandlerThread(THREAD_NAME);
            mGlobalHandlerThread.setDaemon(true);
            mGlobalHandlerThread.start();
        }
        if (mGlobalSingleThread == null) {
            mGlobalSingleThread = new GlobalSingleThread(context,
                    mGlobalHandlerThread.getLooper());
        }
        return mGlobalSingleThread;
    }

    /**
     * stop and cancle the tj thread
     */
    public void stopGlobalSingleThread() {
        this.removeCallbacksAndMessages(null);
        try {
            if (mGlobalHandlerThread != null) {
                mGlobalHandlerThread.join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        mGlobalHandlerThread = null;
        mGlobalSingleThread = null;
    }

    /**
     * start run sth and clear old queue
     * @param doSthRunnable
     */
    public void startRunAndClearQueue(Runnable doSthRunnable) {
        this.removeMessages(MSG_GET_TJ_DATA);
        this.startRunAndQueueUp(doSthRunnable);
    }

    /**
     * start run sth and clear old queue
     * @param doSthRunnable
     */
    public void startRunAndClearQueueForPlay(Runnable doSthRunnable) {
        this.removeMessages(MSG_GET_PLAY_DATA);
        this.startRunAndQueueUpForPlay(doSthRunnable);
    }

    /**
     * 排队等候运行 queue up
     * @param doSthRunnable
     */
    public void startRunAndQueueUp(Runnable doSthRunnable) {
        Message msg = this.obtainMessage(MSG_GET_TJ_DATA);
        msg.obj = doSthRunnable;
        this.sendMessage(msg);
    }

    /**
     * 排队等候运行 queue up
     * @param doSthRunnable
     */
    public void startRunAndQueueUpForPlay(Runnable doSthRunnable) {
        Message msg = this.obtainMessage(MSG_GET_PLAY_DATA);
        msg.obj = doSthRunnable;
        this.sendMessage(msg);
    }

    /**
     * 排队等候运行 queue up
     * @param doSthRunnable
     */
    public void startRunAndQueueUp(Runnable doSthRunnable, int MSG_WHAT) {
        Message msg = this.obtainMessage(MSG_WHAT);
        msg.obj = doSthRunnable;
        this.sendMessage(msg);
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
        case MSG_GET_TJ_DATA:
        case MSG_GET_PLAY_DATA:
            Runnable runnable = (Runnable) msg.obj;
            if (runnable != null) {
                runnable.run();
            }
            break;
        }
    }

}

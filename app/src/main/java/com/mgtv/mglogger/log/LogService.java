package com.mgtv.mglogger.log;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

/**
 * Created by liyang on 2018/4/1.
 * email:liyang3@mgtv.com
 */
public class LogService extends Service {
    private static final String TAG = "LogService";
    private Thread mLogThread;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        MGLog.i(TAG, "LogService onCreate");
        mLogThread = new Thread(new Runnable() {
            @Override
            public void run() {
                LogManager.getInstance().start();
            }
        });
        mLogThread.start();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        MGLog.i(TAG, "LogService onStart:" + this);
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        MGLog.i(TAG, "LogService onDestroy :" + this);
        try {
            mLogThread.interrupt();
            LogManager.getInstance().stopProcess();
        } catch (Exception e) {
            MGLog.i(TAG, e.getMessage());
        }
        stopSelf();
    }
}

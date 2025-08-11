package com.mgtv.tv.base.core.log;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;

/**
 * Created by liyang on 2018/4/1.
 * email:liyang3@mgtv.com
 */
public class LogService extends Service {
    private static final String TAG = "LogService";

    private static boolean sIsNeedstartForeground = true;
    private static boolean sKillProgress;
    private static boolean sStartForegroundOnCreate = true;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        LogManager.getInstance().logI(TAG, "LogService onCreate sIsNeedstartForeground =" + sIsNeedstartForeground);
        if (sStartForegroundOnCreate) {
            startForegroundNotification();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        LogManager.getInstance().logI(TAG, "LogService onStart:" + this);
        if (!sStartForegroundOnCreate) {
            startForegroundNotification();
        }
        //只有logcat进程处于失活状态才会重新启动log收集线程
        if (!LogManager.getInstance().isLogcatAlive()){
            LogManager.getInstance().start();
        }
        if(sKillProgress){// 如果是杀进程的渠道，不自动重启服务
            return START_NOT_STICKY;
        }
        return super.onStartCommand(intent, flags, startId);
    }

    private void startForegroundNotification() {
        // fix bug:TVAPP-2426 【兼容性】安卓9.0上启动出现崩溃，android8.0以上版本，开机自启进程，LogService以
        // 非前台进程启动时会导致应用崩溃
        // 故android8.0以上版本LogService启动后onCreate方法中声明为前台服务
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O && sIsNeedstartForeground) {
            String channelId = String.valueOf(hashCode());
            NotificationChannel channel = new NotificationChannel(channelId, LogService.TAG,
                    NotificationManager.IMPORTANCE_DEFAULT);
            NotificationManager manager = (NotificationManager) getSystemService(
                    Context.NOTIFICATION_SERVICE);
            manager.createNotificationChannel(channel);
            Notification notification = new Notification.Builder(this, channelId).build();
            startForeground(100, notification);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        LogManager.getInstance().logI(TAG, "LogService onDestroy :" + this);
        try {
            LogManager.getInstance().stopProcess();
        } catch (Exception e) {
            LogManager.getInstance().logI(TAG, e.getMessage());
        }
        stopSelf();
    }

    public static void setNeedstartForeground(boolean isNeedstartForeground) {
        LogService.sIsNeedstartForeground = isNeedstartForeground;
    }

    public static void setsKillProgress(boolean killProgress) {
        LogService.sKillProgress = killProgress;
    }

    public static void setStartForegroundOnCreate(boolean startForegroundOnCreate) {
        LogService.sStartForegroundOnCreate = startForegroundOnCreate;
    }
}

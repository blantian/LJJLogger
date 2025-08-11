package com.mgtv.mglogger;

import android.app.Application;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

import com.mgtv.logger.LoggerConfig;
import com.mgtv.logger.MGLogger;
import com.mgtv.logger.MGLoggerStatus;
import com.mgtv.mglogger.log.MGLog;
import com.mgtv.mglogger.log.utils.ContextProvider;
import com.mgtv.tv.base.core.log.LogService;

import java.io.File;
import java.util.ArrayList;

public class MyApplication extends Application {

    private static final String TAG = "MyApplication";
    private static final String FILE_NAME = "mglog";

    public static boolean isNativeLogan = false; // 是否使用Native Logan

    private Thread readThread;

    @Override
    public void onCreate() {
        super.onCreate();
//        readThread = AssetReader.logTextFileAsync(this);
        initLogan(true); // true: native , false: java
    }

    private void initLogan(boolean isNative) {
        if (isNative) {
            File internalDir = new File(getApplicationContext().getFilesDir(), "logcache/");
            if (!internalDir.exists()) {
                boolean isCreated = internalDir.mkdirs();
                if (!isCreated) {
                    Log.e("Logan", "Failed to create directory: " + internalDir.getAbsolutePath());
                }
            }
            // 配置黑名单
            ArrayList<String> blackList = new ArrayList<>();
            blackList.add("MyApplication");
            blackList.add("art");
            blackList.add("IPCThreadState");
            blackList.add("dalvikvm");

            LoggerConfig loggerConfig = LoggerConfig
                    .builder(internalDir.getAbsolutePath(), internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                    .nativeLogCacheSelector(0) // 0: hook, 1: logcat
                    .logcatBlackList(blackList)
                    .build();
            MGLogger.setStatusListener((code, msg) -> {
                Log.i(TAG, "Logger:: code=" + code + " | " + "msg : " + msg);
                if (msg.equals(MGLoggerStatus.MGLOGGER_INIT_STATUS) && code == MGLoggerStatus.MGLOGGER_OK) {
                    Log.i(TAG, "Logger initialized successfully");
                    isNativeLogan = true;
//                    MGLogger.start(); // 如果底层已经hook或者logcat已经开启，这里不需要再调用 start
                } else {
                    Log.e(TAG, "Logger initialization failed with code: " + code);
                    isNativeLogan = false;
                }
            });
            MGLogger.init(loggerConfig);
//            MGLogger.write("write from app ", 1);
        } else {
            ContextProvider.init(this);
            MGLog.initLogManager();
            Intent intent = new Intent(this, LogService.class);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                this.startForegroundService(intent);
            } else {
                this.startService(intent);
            }
        }
    }
}

package com.lt.mglogger;

import android.app.Application;
import android.util.Log;

import com.lt.logger.LoggerConfig;
import com.lt.logger.LJJLogger;
import com.lt.logger.LJJLoggerStatus;

import java.io.File;
import java.util.ArrayList;

public class MyApplication extends Application {

    private static final String TAG = "MyApplication";
    private static final String FILE_NAME = "log";

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
            LJJLogger.setStatusListener((code, msg) -> {
                Log.i(TAG, "Logger:: code=" + code + " | " + "msg : " + msg);
                if (msg.equals(LJJLoggerStatus.MGLOGGER_INIT_STATUS) && code == LJJLoggerStatus.MGLOGGER_OK) {
                    Log.i(TAG, "Logger initialized successfully");
                    isNativeLogan = true;
                } else {
                    Log.e(TAG, "Logger initialization failed with code: " + code);
                    isNativeLogan = false;
                }
            });
            LJJLogger.init(loggerConfig);
//            MGLogger.write("write from app ", 1);
        }
    }
}

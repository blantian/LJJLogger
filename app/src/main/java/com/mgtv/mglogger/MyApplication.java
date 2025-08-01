package com.mgtv.mglogger;

import android.app.Application;
import android.content.Intent;
import android.os.Build;
import android.os.Environment;
import android.util.Log;

import com.mgtv.logger.log.LoggerConfig;
import com.mgtv.logger.log.MGLogger;
import com.mgtv.logger.mglog.LogService;
import com.mgtv.mglogger.log.MGLog;
import com.mgtv.mglogger.log.utils.ContextProvider;

import java.io.File;
import java.util.ArrayList;

public class MyApplication extends Application {

    private static final String TAG = "MyApplication";
    private static final String FILE_NAME = "mglog";

    private String path;

    private Thread readThread;

    @Override
    public void onCreate() {
        super.onCreate();
        path = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "mgtv" + File.separator + FILE_NAME;
        initLogan(true);
        Log.i(TAG, "Logan path: " + path);
//        readThread = AssetReader.logTextFileAsync(this); // 默认路径
    }

    private void initLogan(boolean isNative) {
        if (isNative) {
            File internalDir;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                internalDir = new File(getApplicationContext().getFilesDir(), "logcache/");
            } else {
                internalDir = new File(Environment.getExternalStorageDirectory(),
                        "/Android/data/" + getPackageName() + "/cache/");
            }

            if (!internalDir.exists()) {
                boolean isCreated = internalDir.mkdirs();
                if (!isCreated) {
                    Log.e("Logan", "Failed to create directory: " + internalDir.getAbsolutePath());
                }
            }

            Log.i(TAG, "save path: " + internalDir.getAbsolutePath());

            // 配置黑名单
            ArrayList<String> blackList = new ArrayList<>();
            blackList.add("MyApplication");
            blackList.add("art");
            blackList.add("IPCThreadState");

            LoggerConfig loggerConfig = LoggerConfig
                    .builder(internalDir.getAbsolutePath(), internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                    .nativeLogCacheSelector(1) // 0: hook, 1: logcat
                    .logcatBlackList(blackList)
                    .build();
            MGLogger.setStatusListener((cmd, code) -> Log.i(TAG, "Logger::" + cmd + " | " + "code : " + code));
            MGLogger.init(loggerConfig);

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

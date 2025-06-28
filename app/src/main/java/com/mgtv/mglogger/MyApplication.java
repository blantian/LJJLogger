package com.mgtv.mglogger;

import android.app.Application;
import android.os.Environment;
import android.util.Log;


import androidx.annotation.NonNull;

import com.mgtv.logger.java.Logan;
import com.mgtv.logger.java.LoganConfig;
import com.mgtv.logger.java.OnLoganProtocolStatus;
import com.mgtv.logger.kt.i.ILoggerStatus;
import com.mgtv.logger.kt.log.LoggerConfig;
import com.mgtv.logger.kt.log.MGLogger;

import java.io.File;

public class MyApplication extends Application {

    private static final String TAG = MyApplication.class.getName();
    private static final String FILE_NAME = "logan_v1";

    private String path;

    @Override
    public void onCreate() {
        super.onCreate();
        path = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "mgtv" + File.separator + FILE_NAME;
        initLogan();
    }

    private void initLogan() {
        File internalDir = new File(getApplicationContext().getFilesDir(), "LoganLogs");
        if (!internalDir.exists()) {
            boolean isCreated = internalDir.mkdirs();
            if (!isCreated) {
                Log.e("Logan", "Failed to create directory: " + internalDir.getAbsolutePath());
            }
        }

        LoggerConfig loggerConfig = new LoggerConfig.Builder()
                .putCachePath(internalDir.getAbsolutePath())
                .putLogDir(internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                .build();
        MGLogger.init(loggerConfig, (cmd, code) -> {
            Log.i(TAG, "clogan > cmd : " + cmd + " | " + "code : " + code);
        });

//        LoganConfig config = new LoganConfig.Builder()
//                .setCachePath(internalDir.getAbsolutePath())
//                .setPath(internalDir.getAbsolutePath() + File.separator + FILE_NAME)
//                .setEncryptKey16("0123456789012345".getBytes())
//                .setEncryptIV16("0123456789012345".getBytes())
//                .build();
//        Logan.init(config);
//        Logan.setDebug(true);
//        Logan.setOnLoganProtocolStatus(new OnLoganProtocolStatus() {
//            @Override
//            public void loganProtocolStatus(String cmd, int code) {
//                Log.d(TAG, "clogan > cmd : " + cmd + " | " + "code : " + code);
//            }
//        });
    }
}

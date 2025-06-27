package com.mgtv.mglogger;

import android.app.Application;
import android.os.Environment;
import android.util.Log;


import com.mgtv.logger.Logan;
import com.mgtv.logger.LoganConfig;
import com.mgtv.logger.OnLoganProtocolStatus;

import java.io.File;

public class MyApplication extends Application {

    private static final String TAG = MyApplication.class.getName();
    private static final String FILE_NAME = "logan_v1";

    private String path;

    @Override
    public void onCreate() {
        super.onCreate();
//        path = getApplicationContext().getExternalFilesDir(null).getAbsolutePath()
//                + File.separator + FILE_NAME ;
        path = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "mgtv" + File.separator + FILE_NAME;
        initLogan();
        Logan.w("MyApplication onCreate", 3);
        Logan.w("MyApplication onCreate", 3);
        Logan.w("MyApplication onCreate", 3);
    }

    private void initLogan() {
        File internalDir = new File(getApplicationContext().getFilesDir(), "LoganLogs");
        if (!internalDir.exists()) {
            boolean isCreated = internalDir.mkdirs();
            if (!isCreated) {
                Log.e("Logan", "Failed to create directory: " + internalDir.getAbsolutePath());
            }
        }

        LoganConfig config = new LoganConfig.Builder()
                .setCachePath(internalDir.getAbsolutePath())
                .setPath(internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                .setEncryptKey16("0123456789012345".getBytes())
                .setEncryptIV16("0123456789012345".getBytes())
                .build();
        Logan.init(config);
        Logan.setDebug(true);
        Logan.setOnLoganProtocolStatus(new OnLoganProtocolStatus() {
            @Override
            public void loganProtocolStatus(String cmd, int code) {
                Log.d(TAG, "clogan > cmd : " + cmd + " | " + "code : " + code);
            }
        });
    }
}

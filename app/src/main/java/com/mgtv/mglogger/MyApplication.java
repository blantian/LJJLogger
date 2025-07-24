package com.mgtv.mglogger;

import android.app.Application;
import android.os.Build;
import android.os.Environment;
import android.util.Log;
import com.mgtv.logger.kt.log.LoggerConfig;
import com.mgtv.logger.kt.log.MGLogger;
import com.mgtv.mglogger.log.utils.ContextProvider;

import java.io.File;
import java.util.ArrayList;

public class MyApplication extends Application {

    private static final String TAG = "MyApplication";
    private static final String FILE_NAME = "logan_v1";

    private String path;

    private Thread readThread;

    @Override
    public void onCreate() {
        super.onCreate();
        path = Environment.getExternalStorageDirectory().getAbsolutePath()
                + File.separator + "mgtv" + File.separator + FILE_NAME;
        initLogan();
//        MGLog.initLogManager(getApplicationContext());
        ContextProvider.init(this);
//        MGLogger.w("TAG",1);
        Log.i(TAG, "Logan path: " + path);
        readThread = AssetReader.logTextFileAsync(this); // 默认路径
    }

    private void initLogan() {

        File internalDir ;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            internalDir = new File(getApplicationContext().getFilesDir(), "LoganLogs");
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

        LoggerConfig loggerConfig = new LoggerConfig.Builder()
                .putCachePath(internalDir.getAbsolutePath())
                .putLogDir(internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                .putLogcatBlackList(blackList)
                .putLogCacheS(0) // 0: hook, 1: logcat
                .build();
        MGLogger.init(loggerConfig, (cmd, code) -> {
            Log.i(TAG, "Logger::" + cmd + " | " + "code : " + code);

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
//                MGLog.d(TAG, "clogan > cmd : " + cmd + " | " + "code : " + code);
//            }
//        });
    }
}

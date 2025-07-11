package com.mgtv.mglogger;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import com.mgtv.logger.kt.i.ILoggerStatus;
import com.mgtv.logger.kt.log.Logger;
import com.mgtv.logger.kt.log.MGLogger;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private Thread readThread;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        mg_logger.w("MainActivity onCreate", 3);
//        new Thread(() -> {
//            mg_logger.w("MainActivity onCreate", 1);
//        }).start();
//        throw new RuntimeException("Test Crash");
//        mg_logger.getSystemLogs(1);
//        mg_logger.flush();
    }



    @Override
    protected void onPause() {
        super.onPause();
        MGLogger.w("MainActivity onPause", 3);
        MGLogger.flush();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (readThread != null) readThread.interrupt();  // 防止泄漏
    }
}
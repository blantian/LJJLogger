package com.lt.ljjlogger;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class MultiProcessActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_multi_process);
        initLogger();

        findViewById(R.id.flush).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("MultiProcessActivity", "flush log");
                LJJLogger.flush();
            }
        });

        findViewById(R.id.write).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LJJLogger.write("write from multi process activity ", 1);
                Log.i("MultiProcessActivity", "hello world");
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        LJJLogger.flush();
    }

    private void initLogger() {
        File internalDir = new File(getApplicationContext().getFilesDir(), "logcache/");
        if (!internalDir.exists()) {
            internalDir.mkdirs();
        }
        LoggerConfig loggerConfig = LoggerConfig
                .builder(internalDir.getAbsolutePath(), internalDir.getAbsolutePath() + File.separator + "multi_mglog")
                .nativeLogCacheSelector(1)
                .build();
        LJJLogger.setStatusListener((code, msg) -> {
            if (msg.equals(LJJLoggerStatus.MGLOGGER_INIT_STATUS) && code == LJJLoggerStatus.MGLOGGER_OK) {
                Log.i("MultiProcessActivity", "Logger initialized successfully");
            } else {
                Log.e("MultiProcessActivity", "Logger initialization failed with code: " + code);
            }
        });
        LJJLogger.init(loggerConfig);
    }
}

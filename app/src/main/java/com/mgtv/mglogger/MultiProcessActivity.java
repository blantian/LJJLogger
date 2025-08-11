package com.mgtv.mglogger;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mgtv.logger.LoggerConfig;
import com.mgtv.logger.MGLogger;
import com.mgtv.logger.MGLoggerStatus;

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
                MGLogger.flush();
            }
        });

        findViewById(R.id.write).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MGLogger.write("write from multi process activity ", 1);
                Log.i("MultiProcessActivity", "hello world");
            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        MGLogger.flush();
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
        MGLogger.setStatusListener((code, msg) -> {
            if (msg.equals(MGLoggerStatus.MGLOGGER_INIT_STATUS) && code == MGLoggerStatus.MGLOGGER_OK) {
                Log.i("MultiProcessActivity", "Logger initialized successfully");
            } else {
                Log.e("MultiProcessActivity", "Logger initialization failed with code: " + code);
            }
        });
        MGLogger.init(loggerConfig);
    }
}

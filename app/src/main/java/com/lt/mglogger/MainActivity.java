package com.lt.mglogger;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.lt.logger.LJJLogger;
import com.lt.mglogger.log.utils.LogPrinter;

import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private TextView textView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.flush).setOnClickListener(v -> {
            if (MyApplication.isInitialized) {
                Log.i(TAG, "flush log");
                LJJLogger.flush();
            } else {
                Log.w(TAG, "Native Logan is not enabled, flush operation skipped.");
            }
        });

        findViewById(R.id.write).setOnClickListener(v -> {
            if (MyApplication.isInitialized) {
                LJJLogger.write("write from activity ", 1);
            } else {
                Log.w(TAG, "Native Logan is not enabled, write operation skipped.");
            }
        });

        findViewById(R.id.multi_process).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, MultiProcessActivity.class));
            }
        });


        findViewById(R.id.test_exception).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "Throwing test exception");
                throw new RuntimeException("Test Exception");
            }
        });

        findViewById(R.id.send_log).setOnClickListener(v -> {
            RealSendLogRunnable realSendLogRunnable = new RealSendLogRunnable();
            LJJLogger.sendLog(realSendLogRunnable);
        });

        if (MyApplication.isInitialized) {
            LJJLogger.flush();
        }

        textView = findViewById(R.id.logs_info);
        findViewById(R.id.show_log).setOnClickListener(v -> {
            if (MyApplication.isInitialized) {
                Map<String, Long> map = LJJLogger.getAllFilesInfo();
                if (map == null || map.isEmpty()) {
                    Log.i(TAG, "No log files found.");
                    textView.setText("No log files found.");
                    return;
                }

                StringBuilder sb = new StringBuilder();
                for (Map.Entry<String, Long> entry : map.entrySet()) {
                    sb.append("File: ").append(entry.getKey()).append(", Size: ").append(entry.getValue()).append(" bytes\n");
                }
                textView.setText(sb.toString());
                Log.i(TAG, "LogPrinter started");
            } else {
                Log.w(TAG, "Native Logan is not enabled, LogPrinter operation skipped.");
            }
        });
    }


    @Override
    protected void onResume() {
        super.onResume();
        if (MyApplication.isInitialized) {
            LJJLogger.flush();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (MyApplication.isInitialized) {
            LJJLogger.flush();
        }
        LogPrinter.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
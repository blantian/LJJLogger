package com.mgtv.mglogger;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mgtv.logger.MGLogger;
import com.mgtv.mglogger.log.utils.LogPrinter;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.flush).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (MyApplication.isNativeLogan) {
                    Log.i(TAG, "flush log");
                    MGLogger.flush();
                } else {
                    Log.w(TAG, "Native Logan is not enabled, flush operation skipped.");
                }
            }
        });

        findViewById(R.id.write).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (MyApplication.isNativeLogan) {
                    MGLogger.write("write from activity ", 1);
                } else {
                    Log.w(TAG, "Native Logan is not enabled, write operation skipped.");
                }
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

        findViewById(R.id.print_log).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                LogPrinter.start();
                RealSendLogRunnable realSendLogRunnable = new RealSendLogRunnable();
                MGLogger.sendLog(realSendLogRunnable);
                Log.i(TAG, "LogPrinter started");
            }
        });

        if (MyApplication.isNativeLogan) {
            MGLogger.flush();
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        if (MyApplication.isNativeLogan){
            MGLogger.flush();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (MyApplication.isNativeLogan){
            MGLogger.flush();
        }
        LogPrinter.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
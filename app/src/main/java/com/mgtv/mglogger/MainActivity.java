package com.mgtv.mglogger;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.mgtv.logger.log.MGLogger;
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
                Log.i(TAG, "flush log");
                MGLogger.flush();
            }
        });

        findViewById(R.id.write).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MGLogger.write("write from activity ", 1);
                Log.i(TAG, "hello world");
            }
        });

        findViewById(R.id.multi_process).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, MultiProcessActivity.class));
            }
        });
//        LogPrinter.start();
//        throw new RuntimeException("Test Exception");
    }



    @Override
    protected void onPause() {
        super.onPause();
        MGLogger.flush();
        LogPrinter.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
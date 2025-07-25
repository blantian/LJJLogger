package com.mgtv.mglogger;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.mgtv.logger.kt.i.ILoggerStatus;
import com.mgtv.logger.kt.log.Logger;
import com.mgtv.logger.kt.log.MGLogger;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.flush).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MGLogger.flush();
            }
        });

        findViewById(R.id.write).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                MGLogger.w("hello world", 2);
                Log.i(TAG, "hello world");
            }
        });

//        throw new RuntimeException("Test Exception");
    }



    @Override
    protected void onPause() {
        super.onPause();
//        MGLogger.w("MainActivity onPause", 3);
        MGLogger.flush();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
package com.mgtv.mglogger;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.mgtv.logger.kt.log.Logger;
import com.mgtv.logger.kt.log.MGLogger;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        MGLogger.w("MainActivity onCreate", 3);
        new Thread(() -> {
            MGLogger.w("MainActivity onCreate", 1);
//            for (int i = 0; i < 100; i++) {
//                Log.w("Test", "Hello from Java " + i);
//            }
        }).start();
//        throw new RuntimeException("Test Crash");
        MGLogger.getSystemLogs(1);
//        Log.i("Test", "Hello from Java");
//        for (int i = 0; i < 100; i++) {
//            Log.w("Test", "Hello from Java " + i);
//        }
        MGLogger.flush();
    }


    @Override
    protected void onPause() {
        super.onPause();
        MGLogger.flush();
    }
}
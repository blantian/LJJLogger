package com.lt.ljjlogger.log.utils;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class AssetReader {

    private static final String TAG = "AssetReader";
    private static final String DEFAULT_PATH = "txt/西游记.txt";
    private static final long   INTERVAL_MS  = 50L;

    /** 在新线程读取并打印 */
    public static Thread logTextFileAsync(Context ctx) {
        return logTextFileAsync(ctx, DEFAULT_PATH);
    }

    public static Thread logTextFileAsync(Context ctx, String filePath) {
        Thread t = new Thread(() -> readAndPrint(ctx, filePath), "AssetReaderThread");
        t.start();
        return t;
    }

    private static void readAndPrint(Context ctx, String filePath) {
        AssetManager am = ctx.getAssets();
        try (InputStream is = am.open(filePath);
             BufferedReader br = new BufferedReader(new InputStreamReader(is, "UTF-8"))) {

            String line;
            int lineNum = 1;
            while ((line = br.readLine()) != null) {
                Log.d(TAG, "line " + lineNum++ + ": " + line);
                try {
                    Thread.sleep(INTERVAL_MS); // 每 20 ms 停顿
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }

        } catch (IOException e) {
            Log.e(TAG, "Read asset failed: " + filePath, e);
        }
    }
}


package com.mgtv.logger;

import static android.os.Environment.getExternalStorageDirectory;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class LoggerUtils {
    private static final String TAG = "LoggerUtils";
    private static final SimpleDateFormat sDateFormat = new SimpleDateFormat("yyyy-MM-dd");

    /**
     * 获取当前时间的毫秒数，格式化为 yyyy-MM-dd 的字符串后再转换为毫秒数。
     *
     * @return 当前时间的毫秒数
     */
    public static long getCurrentTime() {
        long currentTime = System.currentTimeMillis();
        long tempTime = 0;
        try {
            String dataStr = sDateFormat.format(new Date(currentTime));
            tempTime = sDateFormat.parse(dataStr).getTime();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return tempTime;
    }

    public static String getDateStr(long time) {
        return sDateFormat.format(new Date(time));
    }

    // 获取SDCard路径
    public static File getExternalCacheDir(Context context) {
        boolean isSDCardExist = false;
        try{
            isSDCardExist = Environment.getExternalStorageState()
                    .equals(android.os.Environment.MEDIA_MOUNTED);
        }catch (Exception e){
            e.printStackTrace();
            /**
             * java.lang.RuntimeException: android.os.DeadSystemException
             * 	at android.os.storage.StorageManager.getVolumeList(StorageManager.java:1171)
             * 	at android.os.Environment$UserEnvironment.getExternalDirs(Environment.java:91)
             * 	at android.os.Environment.getExternalStorageState(Environment.java:896)
             * 	at com.mgtv.tv.base.core.k.b(FileUtils.java:133)
             */
        }
        if (!isSDCardExist || context == null) {
            return null;
        }
        final File externalCacheDir = new File(Environment.getExternalStorageDirectory(),
                "/Android/data/" + context.getPackageName() + "/cache/");
        if (!externalCacheDir.exists()) {
            externalCacheDir.mkdirs();
        }
        // externalCacheDir.mkdirs 失败的情况下使用系统自带方法重新创建
        // context.getExternalCacheDir 会自动调用系统方法创建外部SD卡缓存文件夹
        if (!externalCacheDir.exists() && context.getExternalCacheDir() != null){
            Log.d(TAG,"mkExternalCacheDir failed !reMake,result:"+ externalCacheDir.exists());
        }
        return externalCacheDir;
    }
}

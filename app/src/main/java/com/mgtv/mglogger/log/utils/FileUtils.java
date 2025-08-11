package com.mgtv.mglogger.log.utils;

import android.content.Context;
import android.os.Environment;

import java.io.File;
import java.util.Arrays;
import java.util.Comparator;

public class FileUtils {
    /**
     * 获取应用的内部缓存目录
     *
     * @param context 上下文
     * @return 内部缓存目录
     */
    public static File getExternalCacheDir(Context context) {
        boolean isSDCardExist = Environment.getExternalStorageState()
                .equals(Environment.MEDIA_MOUNTED);
        if (!isSDCardExist) {
            return null;
        }
        final File externalCacheDir = new File(Environment.getExternalStorageDirectory(),
                "/Android/data/" + context.getPackageName() + "/cache/");
        if (!externalCacheDir.exists()) {
            externalCacheDir.mkdirs();
        }
        return externalCacheDir;
    }

    public static File getInternalCacheDir(Context context) {
        if (context == null) {
            return null;
        }
        File dir = context.getCacheDir();
        if (dir == null) {
            return null;
        }
        if (!dir.exists()) {
            dir.mkdirs();
        }
        return dir;
    }


    public static File getFileByAbsolutePath(String path) {
        File file = new File(path);
        if (!file.exists()) {
            try {
                file.createNewFile();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return file;
    }

    public static File[] orderByDate(String dirPath) {
        File file = new File(dirPath);
        if (!file.exists() || !file.isDirectory()) {
            return null;
        }
        File[] fs = file.listFiles();
        if (fs == null || fs.length <= 0) {
            return null;
        }
        Arrays.sort(fs, new Comparator<File>() {
            public int compare(File f1, File f2) {
                long diff = f1.lastModified() - f2.lastModified();
                if (diff > 0) {
                    return 1;
                } else if (diff == 0) {
                    return 0;
                } else {
                    return -1;
                }
            }
        });
        return fs;
    }
}

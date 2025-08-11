package com.mgtv.mglogger.log;

import android.content.Context;
import android.util.Log;

import com.mgtv.mglogger.log.utils.ContextProvider;
import com.mgtv.mglogger.log.utils.FileUtils;
import com.mgtv.mglogger.log.utils.StringUtils;
import com.mgtv.mglogger.log.utils.ThreadUtils;
import com.mgtv.tv.base.core.log.ILogListener;
import com.mgtv.tv.base.core.log.ILogProxy;
import com.mgtv.tv.base.core.log.LogManager;

import java.io.File;
import java.io.OutputStream;
import java.util.List;

public class MGLog {

    private static ILogListener mLogListener;

    public static void initLogManager() {
        ILogProxy proxy = new ILogProxy() {
            @Override
            public Context getAppContext() {
                return ContextProvider.getApplicationContext();
            }

            @Override
            public String getPackageName() {
                return ContextProvider.getApplicationContext().getPackageName();
            }

            @Override
            public void killLogcatProcess() {
//                ContextProvider.getApplicationContext().killLogcatProcess(getPackageName());

            }

            @Override
            public void initMGLogListener(ILogListener iLogListener) {
                MGLog.initMGLogListener(iLogListener);
            }

            @Override
            public void d(String s, String s1) {
                MGLog.d(s,s1);
            }

            @Override
            public void i(String s, String s1) {
                MGLog.i(s,s1);
            }

            @Override
            public boolean isDebugOn() {
                return true;
            }

            @Override
            public boolean isEqualsNull(String s) {
                return true;
            }

            @Override
            public String getCurrentTime(String format) {
                return String.valueOf(System.currentTimeMillis());
            }

            @Override
            public File getExternalCacheDir() {
                return FileUtils.getExternalCacheDir(ContextProvider.getApplicationContext());
            }

            @Override
            public File getInternalCacheDir() {
                return FileUtils.getInternalCacheDir(ContextProvider.getApplicationContext());
            }

            @Override
            public File getFileByAbsolutePath(String path) {
                return FileUtils.getFileByAbsolutePath(path);
            }

            @Override
            public File[] orderByDate(String dirPath) {
                return FileUtils.orderByDate(dirPath);
            }

            @Override
            public void closeIOStream(OutputStream outputStream) {
//                IOUtils.closeStream(outputStream);
            }

            @Override
            public void doZipFilesWithPassword(File zipFile, char[] chars, List<File> list, String appendName, File file) {
                if (zipFile == null || list == null){
                    return;
                }
                chars = null;
//                if (!StringUtils.equalsNull(appendName)){
//                    try {
//                        ZipUtil.doZipFilesWithPassword(new FileOutputStream(zipFile), chars, list
//                                , appendName, file);
//                    } catch (FileNotFoundException e) {
//                        e.printStackTrace();
//                    }
//                } else {
//                    // 文件拼接名为空时表示不需要拼接文件，直接压缩即可
//                    ZipUtil.doZipFileListWithPassword(list,zipFile.getAbsolutePath(),chars);
//                }
            }

            @Override
            public void runInThread(Runnable runnable) {
                ThreadUtils.startRunLogWriteSingleThread(runnable);
            }
        };
        LogManager.getInstance().init(proxy);
    }


    public static void i(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        i(tag, msg, null);
    }

    public static void e(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        e(tag, msg, null);
    }

    public static void v(String tag, String msg) {

        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        v(tag, msg, null);
    }

    public static void d(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        d(tag, msg, null);
    }

    public static void w(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        w(tag, msg, null);
    }


    public static void w(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.w(tag, msg, tr);
            } else {
//                if (sFilterMac) {
//                    msg = filterMsg(msg);
//                }
                Log.w(tag, msg, tr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void i(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.i(tag, msg, tr);
            } else {
//                if (sFilterMac) {
//                    msg = filterMsg(msg);
//                }
                Log.i(tag, msg, tr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void e(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.e(tag, msg, tr);
            } else {
//                if (sFilterMac) {
//                    msg = filterMsg(msg);
//                }
                Log.e(tag, msg, tr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void v(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.v(tag, msg, tr);
            } else {
                // debug版本才能输出verbose级别日志
                if (true) {
                    Log.v(tag, msg, tr);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void d(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.d(tag, msg, tr);
            } else {
                // debug版本才能输出debug级别日志
                if (true) {
                    Log.d(tag, msg, tr);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public static void initMGLogListener(ILogListener logListener) {
        mLogListener = logListener;
    }

}

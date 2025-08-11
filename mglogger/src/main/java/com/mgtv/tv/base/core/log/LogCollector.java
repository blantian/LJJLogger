package com.mgtv.tv.base.core.log;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

/**
 * 日志收集器，持续收集应用日志
 * 1.清除日志缓存
 * 2.杀死应用程序已开启的Logcat进程防止多个进程写入一个日志文件
 * 3.开启日志收集进程
 */
public class LogCollector extends BaseLogCollector {

    private static final long MAX_LOG_DIR_LENGTH = 2 * 1024 * 1024;// 每个日志文件logcat大小限制为1M
    private static final int MAX_LOG_FILE_SIZE = 5;// 最多五个文件记录日志
    private static final long CACHE_LOG_LENGTH = 512 * 1024;// 最多512KB
    private static final int MAX_FATAL_COUNT = 3;
    private static final String PATH_TRACE = "/data/anr/traces.txt";

    private int mFatalCount;
    private StringBuffer mAppLogBuffer = new StringBuffer();
    // logcat进程是否存活
    private boolean mLogcatAlive;
    private List<String> mPidList;

    public LogCollector(@NonNull String logDir, @NonNull String logPrefix,
                        @NonNull String logAppendName, @NonNull String logZipPath,
                        @NonNull String tag) {
        super(logDir, logPrefix, logAppendName, logZipPath, PATH_TRACE,
                MAX_LOG_DIR_LENGTH, MAX_LOG_FILE_SIZE, tag);
        mPidList = new ArrayList<>();
        mPidList.add(String.valueOf(android.os.Process.myPid()));
    }

    @Override
    public void start() {
        super.start();
        mLogcatAlive = true;
    }

    /**
     * logcat进程是否存活
     *
     * @return
     */
    public boolean isLogcatAlive() {
        return mLogcatAlive;
    }

    /**
     * 添加应用内多进程的对应id
     *
     * @param pid
     */
    public void addPid(String pid) {
        if (LogManager.getInstance().isEqualsNull(pid) || mPidList.contains(pid)) {
            return;
        }
        mPidList.add(pid);
    }

    @Override
    protected void run() {
        LogManager.getInstance().killLogcatProcess();
        String[] cmds = LogManager.getCmd();
        String fatalInfo = "";
        try {
            mLogcatAlive = true;
            // 将内存缓存日志写入文件
            writeAppLog();
            resetBufferReader(cmds);
            String line;
            while (true) {
                line = mReader.readLine();
                if (mFatalCount >= MAX_FATAL_COUNT) {
                    // 连续几次重新启动log进程都异常，退出使用兜底方案
                    fatalInfo += "line == null and restart error !";
                    break;
                }
                if (line == null) {
                    // log进程异常退出，重启
                    mFatalCount++;
                    clearLogCache();
                    resetBufferReader(cmds);
                    continue;
                } else {
                    mFatalCount = 0;
                }
                checkLogSize(line);
                if (isMineProcess(line)) {
                    String content = line + "\r\n";
                    mFos.write(content.getBytes());
                    mFos.flush();
                }
            }
        } catch (Exception e) {
            fatalInfo += "recordLog failed" + e;
            LogManager.getInstance().logI(mTag, "recordLog failed" + e);
        } finally {
            stop();
            mLogcatAlive = false;
            // logcat进程收集日志异常后，通过其它方式收集app自身打印日志
            writeAppLog();
            LogManager.getInstance().logI(mTag, "recordLog end!fatalInfo :" + fatalInfo);
        }
    }

    /**
     * 每次记录日志之前先清除日志的缓存, 不然会在两个日志文件中记录重复的日志
     */
    private void clearLogCache() {
        Process proc = null;
        List<String> commandList = new ArrayList<>();
        commandList.add("logcat");
        commandList.add("-c");
        try {
            proc = Runtime.getRuntime().exec(commandList.toArray(new String[commandList.size()]));
            if (proc.waitFor() != 0) {
                LogManager.getInstance().logD(mTag, "clearLogCache proc.waitFor() != 0");
            }
        } catch (Exception e) {
            LogManager.getInstance().logI(mTag, "clearLogCache failed" + e);
        } finally {
            try {
                if (proc != null) {
                    proc.destroy();
                }
            } catch (Exception e) {
                LogManager.getInstance().logI(mTag, "clearLogCache failed" + e);
            }
        }
    }


    /**
     * 是否是当前应用内的日志
     *
     * @param line
     * @return
     */
    private boolean isMineProcess(@NonNull String line) {
        for (String pid : mPidList) {
            if (line.contains(pid)) {
                return true;
            }
        }
        return false;
    }

    /**
     * 写入app日志
     */
    private void writeAppLog() {
        // 如果logcat进程存活，则无须单独收集app日志
        if (mLogcatAlive) {
            saveLog2File();
            LogManager.getInstance().initMGLogListener(null);
            return;
        }
        LogManager.getInstance().initMGLogListener(new ILogListener() {
            @Override
            public void v(String tag, String msg, Throwable tr) {
                if (!LogManager.getInstance().isDebugOn()) {
                    return;
                }
                Log.v(tag, msg, tr);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("V", tag, msg);
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void d(String tag, String msg, Throwable tr) {
                if (!LogManager.getInstance().isDebugOn()) {
                    return;
                }
                Log.d(tag, msg, tr);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("D", tag, msg);
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void i(String tag, String msg, Throwable tr) {
                Log.i(tag, msg, tr);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("I", tag, msg);
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void w(String tag, String msg, Throwable tr) {
                Log.w(tag, msg, tr);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("W", tag, msg);
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void w(String tag, Throwable throwable) {
                Log.w(tag, throwable);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("W", tag, "");
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void e(String tag, Throwable throwable) {
                Log.e(tag, "", throwable);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("E", tag, "");
                } else {
                    saveLog2File();
                }
            }

            @Override
            public void e(String tag, String msg, Throwable tr) {
                Log.e(tag, msg, tr);
                if (mAppLogBuffer.length() <= CACHE_LOG_LENGTH) {
                    cacheLog2Mem("E", tag, msg);
                } else {
                    saveLog2File();
                }
            }
        });
    }

    private void cacheLog2Mem(String level, String tag, String msg) {
        mAppLogBuffer.append(LogManager.getInstance().getCurrentTime(
                LogFileHelper.FORMAT_YMDHMS));
        mAppLogBuffer.append(" ");
        mAppLogBuffer.append(LogManager.getInstance().getPackageName());
        mAppLogBuffer.append(" ");
        mAppLogBuffer.append(level);
        mAppLogBuffer.append("/");
        mAppLogBuffer.append(tag);
        mAppLogBuffer.append(":");
        mAppLogBuffer.append(" ");
        mAppLogBuffer.append(msg);
        mAppLogBuffer.append("\n\n");
    }

    private void saveLog2File() {
        final String log = mAppLogBuffer.toString();
        mAppLogBuffer.delete(0, mAppLogBuffer.length());
        if (log.length() <= 0) {
            return;
        }
        LogManager.getInstance().runInThread(new Runnable() {
            @Override
            public void run() {
                writeLog(log);
            }
        });
    }
}
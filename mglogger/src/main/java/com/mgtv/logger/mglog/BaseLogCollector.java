package com.mgtv.logger.mglog;


import androidx.annotation.NonNull;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * 日志收集器基类
 * Created by wukeqi on 2023/11/28.
 * keqi@mgtv.com
 */
public abstract class BaseLogCollector {

    protected String mTag;
    protected @NonNull
    LogFileHelper mLogFileHelper;
    protected BufferedReader mReader;
    protected FileOutputStream mFos;
    protected File mFile;
    protected Process mLogProcess;
    protected Thread  mThread;
    private long mMaxLogLength;
    private int mMaxLogDirCount;

    public BaseLogCollector(@NonNull String logDir, @NonNull String logPrefix,
                            @NonNull String logAppendName, @NonNull String logZipPath,String trancePath,
                            long maxLogLength, int maxLogDirCount, @NonNull String tag) {
        mTag = tag;
        mMaxLogLength = maxLogLength;
        mMaxLogDirCount = maxLogDirCount;
        mLogFileHelper = new LogFileHelper(logDir, logPrefix, logAppendName, logZipPath, trancePath, tag);
        if (mLogFileHelper.getLogFileCount() <= 0) {
            // 文件夹下不包含文件时，创建一个
            mLogFileHelper.createFile();
        }
    }

    /**
     * 启动日志收集
     */
    public void start() {
        mThread= new Thread(mTag) {
            @Override
            public void run() {
                super.run();
                BaseLogCollector.this.run();
            }
        };
        if(LogManager.isDegradeLogRecordLevel()){// 日志收集降级的场景设置下线程优先级为最低
            mThread.setPriority(Thread.MIN_PRIORITY);
        }
        mThread.start();
    }

    /**
     * 停止日志收集
     */
    public void stop(){
        try {
            if (mLogProcess != null) {
                mLogProcess.destroy();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        LogManager.getInstance().closeIOStream(mFos);
    }


    /**
     * 将日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     *
     * @return 返回日志压缩文件的绝对路径
     */
    public String appendZipLog() {
        return mLogFileHelper.appendZipLog();
    }

    /**
     * 日志收集
     */
    protected abstract void run();

    /**
     * 获取最新的日志文件名称
     *
     * @return
     */
    protected String getLatestLogPath() {
        File[] logFiles = mLogFileHelper.getLogFiles();
        if (logFiles == null) {
            return null;
        }
        int size = logFiles.length;
        if (size <= 0) {
            return null;
        }
        File file = logFiles[size - 1];
        return file == null ? null : file.getAbsolutePath();
    }

    protected void resetBufferReader(String[] cmds) throws IOException {
        String logPath = getLatestLogPath();
        Runtime runtime = Runtime.getRuntime();
        if (mReader != null) {
            mReader.close();
        }
        if (mLogProcess != null) {
            mLogProcess.destroy();
        }
        LogManager.getInstance().closeIOStream(mFos);
        mLogProcess = runtime.exec(cmds);
        mReader = new BufferedReader(new InputStreamReader(mLogProcess.getInputStream()));
        mFile = LogManager.getInstance().getFileByAbsolutePath(logPath);
        mFos = new FileOutputStream(mFile, true);
    }

    protected void checkLogSize(@NonNull String line) throws FileNotFoundException {
        if (mFile.length() + line.length() > mMaxLogLength) {
            // 当前文件内存即将超出界限
            if (mLogFileHelper.getLogFileCount() >= mMaxLogDirCount) {
                // 文件已经全部超出界限，矫正文件（删除创建日期最远的文件）
                mLogFileHelper.correctFile();
            }
            mFile = mLogFileHelper.createFile();
            LogManager.getInstance().closeIOStream(mFos);
            mFos = new FileOutputStream(mFile, true);
        }
    }


    /**
     * 写日志,在用户线程中调用
     *
     * @param content
     */
    public void writeLog(String content) {
        if (LogManager.getInstance().isEqualsNull(content)) {
            return;
        }
        String latestLogPath = getLatestLogPath();
        if (LogManager.getInstance().isEqualsNull(latestLogPath)) {
            return;
        }
        FileOutputStream fos = null;
        try {
            File file = LogManager.getInstance().getFileByAbsolutePath(latestLogPath);
            if (file.length() + content.length() > mMaxLogLength) {
                // 当前文件内存即将超出界限
                if (mLogFileHelper.getLogFileCount() >= mMaxLogDirCount) {
                    // 文件已经全部超出界限，矫正文件（删除创建日期最远的文件）
                    mLogFileHelper.correctFile();
                }
                file = mLogFileHelper.createFile();
            }
            fos = new FileOutputStream(file, true);
            fos.write(content.getBytes());
            fos.flush();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            LogManager.getInstance().closeIOStream(fos);
        }
    }
}
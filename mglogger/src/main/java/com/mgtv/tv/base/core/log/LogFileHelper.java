package com.mgtv.tv.base.core.log;


import androidx.annotation.NonNull;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

/**
 * 日志文件处理帮助类
 * Created by wukeqi on 2023/11/28.
 * keqi@mgtv.com
 */
public class LogFileHelper {

    public static final String FORMAT_YMDHMS = "yyyy-MM-dd HH-mm-ss";
    private static final String PASS_TIME_FORMAT = "yyyyMMdd";
    private static final String LOG_FILE_POSTFIX = ".txt";

    private String mLogDir;
    private String mLogPrefix;
    private String mLogAppendName;
    private String mLogZipPath;
    private String mTag;
    private String mTrancePath;

    public LogFileHelper(@NonNull String logDir, @NonNull String logPrefix,
                         @NonNull String logAppendName, @NonNull String logZipPath, String trancePath,
                         @NonNull String tag) {
        mLogDir = logDir;
        mLogPrefix = logPrefix;
        mLogAppendName = logAppendName;
        mLogZipPath = logZipPath;
        mTag = tag;
        mTrancePath = trancePath;
    }

    public static String appendZipLog(String zipPath, String zipAppendName, List<File> logFiles,
                                      String tracePath, char[] zipPass) {
        File zipFile;
        try {
            zipFile = new File(zipPath);
            if (zipFile.exists()) {
                // 压缩文件已存在时，删除
                zipFile.delete();
            }
            zipFile.createNewFile();
            // 上传log日志和trace文件
            if (logFiles != null) {
                LogManager.getInstance().doZipFilesWithPassword(zipFile, zipPass, logFiles,
                        zipAppendName, tracePath == null ? null : new File(tracePath));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return zipPath;
    }

    public static String getLogZipPath(String pathName) {
        String zipPath = null;
        File externalCacheDir = LogManager.getInstance().getExternalCacheDir();
        if (externalCacheDir != null && externalCacheDir.exists() && externalCacheDir.canWrite()) {
            zipPath = externalCacheDir.getAbsolutePath() + pathName;
        } else if (LogManager.getInstance().getInternalCacheDir() != null) {
            zipPath =
                    LogManager.getInstance().getInternalCacheDir().getAbsolutePath() + pathName;
        }

        return zipPath;
    }

    /**
     * 获取日志文件夹目录
     *
     * @return
     */
    private String getLogDir() {
        StringBuilder sb = new StringBuilder();
        File externalCacheDir = LogManager.getInstance().getExternalCacheDir();
        if (externalCacheDir != null && externalCacheDir.exists() && externalCacheDir.canWrite()) {
            sb.append(externalCacheDir.getAbsolutePath());
        } else if (LogManager.getInstance().getInternalCacheDir() != null) {
            sb.append(LogManager.getInstance().getInternalCacheDir().getAbsolutePath());
        } else {
            return null;
        }
        sb.append(File.separator);
        sb.append(mLogDir);
        return sb.toString();
    }

    /**
     * 创建日志文件的绝对路径地址
     * 以当前时间作为文件名的标志
     *
     * @return
     */
    private String createPath() {
        StringBuilder builder = new StringBuilder();
        builder.append(getLogDir()).append(File.separator);
        builder.append(mLogPrefix);
        builder.append(LogManager.getInstance().getCurrentTime(FORMAT_YMDHMS));
        builder.append(LOG_FILE_POSTFIX);
        return builder.toString();
    }

    /**
     * 在日志文件夹下创建日志文件
     * 日志文件有上级文件夹，所以需要先判断上级文件夹是否存在
     */
    File createFile() {
        File file = new File(createPath());
        File fileParent = file.getParentFile();
        if (!fileParent.exists()) {
            fileParent.mkdirs();
        }
        try {
            file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return file;
    }

    /**
     * 矫正日志文件夹下的文件
     * 删除创建日期最远的文件
     */
    void correctFile() {
        File[] logFiles = getLogFiles();
        if (logFiles == null) {
            return;
        }
        int size = logFiles.length;
        if (size <= 0) {
            return;
        }
        File lastLogFile = logFiles[0];
        if (lastLogFile.exists()) {
            LogManager.getInstance().logD(mTag,
                    "log文件数目超过总数，删除创建日期最远的文件 path:" + lastLogFile.getAbsolutePath());
            lastLogFile.delete();
        }
    }

    /**
     * 将日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     *
     * @return 返回日志压缩文件的绝对路径
     */
    String appendZipLog() {
        File[] files = getLogFiles();
        return appendZipLog(getLogZipPath(), mLogAppendName,files == null ? null : Arrays.asList(files),
                getTraceFilePath(), createZipPass());
    }

    /**
     * 获取log文件压缩的路径
     *
     * @return
     */
    private String getLogZipPath() {
        return getLogZipPath(mLogZipPath);
    }

    /**
     * 获取文件夹下的所有文件的个数
     */
    int getLogFileCount() {
        File[] logFiles = getLogFiles();
        return logFiles == null ? 0 : logFiles.length;
    }

    private char[] createZipPass() {
        String date = LogManager.getInstance().getCurrentTime(PASS_TIME_FORMAT);
        return !LogManager.getInstance().isEqualsNull(date) ? date.toCharArray() : null;
    }

    /**
     * 获取文件夹下的所有文件
     *
     * @return
     */
    File[] getLogFiles() {
        return LogManager.getInstance().orderByDate(getLogDir());
    }

    private String getTraceFilePath() {
        return mTrancePath;
    }
}
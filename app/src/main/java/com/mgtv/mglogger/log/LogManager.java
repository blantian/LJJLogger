package com.mgtv.mglogger.log;

import android.content.Context;


import com.mgtv.mglogger.FileUtils;
import com.mgtv.mglogger.log.utils.StringUtils;
import com.mgtv.mglogger.log.utils.ContextProvider;
import com.mgtv.mglogger.log.utils.IOUtils;
import com.mgtv.mglogger.log.utils.ThreadUtils;
import com.mgtv.mglogger.log.utils.TimeUtils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;

/**
 * Created by wukeqi on 2018/4/8.
 * keqi@mgtv.com
 */

public class LogManager {
    private static final LogManager ourInstance = new LogManager();
    private static final String TAG = "LogManager";
    private static final long MAX_LOG_DIR_LENGTH = 1024 * 1024;// 每个日志文件logcat大小限制为1M
    private static final int MAX_LOG_FILE_SIZE = 5;// 最多五个文件记录日志
    private static final int BUFFER_SIZE = 16 * 1024;// 缓存日志大小

    private LogFileHelper mLogFileHelper;

    private Process mLogProcess;

    private boolean mLogcatEnable = false;// logcat是否可用

    private StringBuffer mLogBuffer = new StringBuffer();

    private LogManager() {
        init();
    }

    public static LogManager getInstance() {
        return ourInstance;
    }

    private void init() {
        MGLog.i(TAG, "LogManager init ---");
        mLogFileHelper = new LogFileHelper();
        if (mLogFileHelper.getLogFileCount() <= 0) {
            // 文件夹下不包含文件时，创建一个
            mLogFileHelper.createFile();
        }
    }

    /**
     * 开启日志输入至文件中
     */
    public void start() {
        String latestLogPath = getLatestLogPath();
        if (StringUtils.equalsNull(latestLogPath)) {
            return;
        }
        recordLog(latestLogPath);
    }

    /**
     * 获取最新的日志文件名称
     * @return
     */
    private String getLatestLogPath() {
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

    /**
     * 打印日志到文件中
     * @param logPath
     */
    private void recordLog(String logPath) {
        MGLog.i(TAG, "LogService recordLog start");
        String[] cmds = { "logcat", "-c" };
        String shellCmd = "logcat -v time -s  *:v | grep \"(" + android.os.Process.myPid() + ")\"";
        Runtime runtime = Runtime.getRuntime();
        BufferedReader reader = null;
        FileOutputStream fos = null;
        try {
            if (mLogProcess != null){
                mLogProcess.destroy();
            }
            runtime.exec(cmds).waitFor();
            mLogcatEnable = true;
            mLogProcess = runtime.exec(shellCmd);
            reader = new BufferedReader(new InputStreamReader(mLogProcess.getInputStream()));

            File file = FileUtils.getFileByAbsolutePath(logPath);
            fos = new FileOutputStream(file, true);
            String line;
            while ((line = reader.readLine()) != null) {
                if (file.length() + line.length() > MAX_LOG_DIR_LENGTH) {
                    // 当前文件内存即将超出界限
                    if (mLogFileHelper.getLogFileCount() >= MAX_LOG_FILE_SIZE) {
                        // 文件已经全部超出界限，矫正文件（删除创建日期最远的文件）
                        mLogFileHelper.correctFile();
                    }
                    file = mLogFileHelper.createFile();
                    IOUtils.closeStream(fos);
                    fos = new FileOutputStream(file, true);
                }
                if (line.contains(String.valueOf(android.os.Process.myPid()))) {
                    String content = line + "\r\n";
                    fos.write(content.getBytes());
                    fos.flush();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            IOUtils.closeStream(reader);
            IOUtils.closeStream(fos);
        }
        MGLog.i(TAG, "LogService recordLog end");
        //无法通过logcat收集日志，将mLogcatEnable置为false
        mLogcatEnable = false;
    }

    /**
     * 缓存日志
     * @param message
     */
    public void bufferLog(String message) {
        if (mLogcatEnable || StringUtils.equalsNull(message)) {
            return;
        }
        mLogBuffer.append(message);
        int length = mLogBuffer.length();
        if (length >= BUFFER_SIZE) {
            final String buffer = mLogBuffer.toString();
            mLogBuffer.delete(0, length);
            ThreadUtils.startRunLogWriteSingleThread(new Runnable() {
                @Override
                public void run() {
                    writeLog(buffer);
                }
            });
        }
    }

    /**
     * 写日志,在用户线程中调用
     * @param content
     */
    public void writeLog(String content){
        if (StringUtils.equalsNull(content)){
            return;
        }
        String latestLogPath = getLatestLogPath();
        if (StringUtils.equalsNull(latestLogPath)) {
            return;
        }
        FileOutputStream fos = null;
        try {
            File file = FileUtils.getFileByAbsolutePath(latestLogPath);
            if (file.length() + content.length() > MAX_LOG_DIR_LENGTH) {
                // 当前文件内存即将超出界限
                if (mLogFileHelper.getLogFileCount() >= MAX_LOG_FILE_SIZE) {
                    // 文件已经全部超出界限，矫正文件（删除创建日期最远的文件）
                    mLogFileHelper.correctFile();
                }
                file = mLogFileHelper.createFile();
            }
            fos = new FileOutputStream(file, true);
            fos.write(content.getBytes());
            fos.flush();
        }catch (Exception e){
            e.printStackTrace();
        }finally {
            IOUtils.closeStream(fos);
        }
    }

    /**
     * 销毁log进程
     */
    public void stopProcess(){
        if (mLogProcess != null){
            mLogProcess.destroy();
            MGLog.d(TAG,"destroy logcat Process");
        }
    }

    /**
     * 将日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     * @return 返回日志压缩文件的绝对路径
     */
    public String appendZipLog() {
        return mLogFileHelper.appendZipLog();
    }

    private static final class LogFileHelper {

        private static final String LOG_DIR = "logcat";
        private static final String LOG_FILE_PREFIX = "log";
        private static final String LOG_FILE_POSTFIX = ".txt";
        private static final String LOG_APPEND_FILE_NAME = "mgtvLog.txt";
        private static final String LOG_ZIP_FILE_PATH = "/log.zip";
        private static final String FORMAT_YMDHMS = "yyyy-MM-dd HH-mm-ss";
        private static final String PATH_TRACE = "/data/anr/traces.txt";
        private static final String LOG_ZIP_TRACE_NAME = "traces.txt";

        private Context mContext;

        private LogFileHelper() {
            mContext = ContextProvider.getApplicationContext();
        }

        /**
         * 获取日志文件夹目录
         * @return
         */
        String getLogDir() {
            StringBuilder sb = new StringBuilder();
            File externalCacheDir = FileUtils.getExternalCacheDir(mContext);
            if (externalCacheDir != null && externalCacheDir.exists()) {
                sb.append(externalCacheDir.getAbsolutePath());
            } else if (FileUtils.getInternalCacheDir(mContext) != null) {
                sb.append(FileUtils.getInternalCacheDir(mContext).getAbsolutePath());
            } else {
                return null;
            }
            sb.append(File.separator);
            sb.append(LOG_DIR);
            return sb.toString();
        }

        /**
         * 创建日志文件的绝对路径地址
         * 以当前时间作为文件名的标志
         * @return
         */
        private String createPath() {
            StringBuilder builder = new StringBuilder();
            builder.append(getLogDir()).append(File.separator);
            builder.append(LOG_FILE_PREFIX);
            builder.append(TimeUtils.transformToString(TimeUtils.getCurrentTime(), FORMAT_YMDHMS));
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
                MGLog.d(TAG, "log文件数目超过总数，删除创建日期最远的文件 path:" + lastLogFile.getAbsolutePath());
                lastLogFile.delete();
            }
        }

        /**
         * 将日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
         * @return 返回日志压缩文件的绝对路径
         */
        String appendZipLog() {
            OutputStream out = null;
            String zipPath = getLogZipPath();
            File zipFile = null;
            try {
                zipFile = new File(zipPath);
                if (zipFile.exists()) {
                    // 压缩文件已存在时，删除
                    zipFile.delete();
                }
                zipFile.createNewFile();
                out = new FileOutputStream(zipFile);
            } catch (IOException e) {
                e.printStackTrace();
            }
            // 上传log日志和trace文件
            File[] logFiles = getLogFiles();
//            if (logFiles != null) {
//                FileUtils.toAppendZip(out, Arrays.asList(logFiles), LOG_APPEND_FILE_NAME,
//                        new File(getTraceFilePath()));
//            }
            return zipPath;
        }

        /**
         * 获取log文件压缩的路径
         * @return
         */
        String getLogZipPath() {
            String zipPath = null;
            File externalCacheDir = FileUtils.getExternalCacheDir(mContext);
            if (externalCacheDir != null && externalCacheDir.exists()) {
                zipPath = externalCacheDir.getAbsolutePath() + LOG_ZIP_FILE_PATH;
            } else if (FileUtils.getInternalCacheDir(mContext) != null) {
                zipPath = FileUtils.getInternalCacheDir(mContext).getAbsolutePath()
                        + LOG_ZIP_FILE_PATH;
            }

            return zipPath;
        }

        /**
         * 获取文件夹下的所有文件的个数
         */
        private int getLogFileCount() {
            File[] logFiles = getLogFiles();
            return logFiles == null ? 0 : logFiles.length;
        }

        /**
         * 获取文件夹下的所有文件
         * @return
         */
        private File[] getLogFiles() {
            return FileUtils.orderByDate(getLogDir());
        }

        private String getTraceFilePath() {
            return PATH_TRACE;
        }
    }
}

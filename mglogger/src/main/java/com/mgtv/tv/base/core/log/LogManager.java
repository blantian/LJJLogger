package com.mgtv.tv.base.core.log;

import android.content.Context;

import com.mgtv.tv.base.core.log.listener.ILogStatusCallback;

import java.io.File;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by wukeqi on 2018/4/8.
 * keqi@mgtv.com
 */

public class LogManager {
    private static final LogManager ourInstance = new LogManager();
    private static final String TAG = "LogManager";

    /**
     * 通用日志配置信息
     */
    private static final String LOG_DIR = "logcat";
    private static final String LOG_FILE_PREFIX = "log";
    private static final String LOG_APPEND_FILE_NAME = "mgtvLog.txt";
    private static final String LOG_ZIP_FILE_PATH = "/log.zip";

    /**
     * 播放器日志配置信息
     */
    private static final String PLAYER_LOG_DIR = "playerlog";
    private static final String PLAYER_LOG_FILE_PREFIX = "player";
    private static final String PLAYER_LOG_APPEND_FILE_NAME = "playerLog.txt";
    private static final String PLAYER_LOG_ZIP_FILE_PATH = "/playerLog.zip";

    /**
     * 崩溃日志配置信息
     */
    private static final String CRASH_LOG_DIR = "crashlog";
    private static final String CRASH_LOG_FILE_PREFIX = "crash";
    private static final String CRASH_LOG_APPEND_FILE_NAME = "crashLog.txt";
    private static final String CRASH_LOG_ZIP_FILE_PATH = "/crashLog.zip";

    private static final String ALL_LOG_ZIP_FILE_PATH = "/allLog.zip";

    private static boolean sDegradeLogRecordLevel = false;
    private LogCollector mLogCollector;
    private OnceLogCollector mPlayerLogCollector;
    private OnceLogCollector mCrashLogCollector;

    private ILogProxy mProxy;

    private LogManager() {

    }

    public static LogManager getInstance() {
        return ourInstance;
    }

    public static void setDegradeLogRecordLevel(boolean isDegrade) {
        sDegradeLogRecordLevel = isDegrade;
    }

    public static boolean isDegradeLogRecordLevel(){
        return sDegradeLogRecordLevel;
    }

    public static String[] getCmd() {
        List<String> commandList = new ArrayList<>();
        commandList.add("logcat");
        commandList.add("-v");
        commandList.add("threadtime");
        if (sDegradeLogRecordLevel) {
            commandList.add("*:i");
            commandList.add("--pid=" + android.os.Process.myPid());
        } else {
            // 不指定进程，收集所有日志消息（一般是当前应用加系统日志）
            commandList.add("*:v");
        }
        return commandList.toArray(new String[commandList.size()]);
    }

    /**
     * 初始化,设置外部代理处理
     *
     * @param proxy
     */
    public void init(ILogProxy proxy) {
        mProxy = proxy;
        logI(TAG, "LogManager init ---");
        mLogCollector = new LogCollector(LOG_DIR, LOG_FILE_PREFIX, LOG_APPEND_FILE_NAME,
                LOG_ZIP_FILE_PATH, TAG);
    }

    /**
     * logcat进程是否存活
     *
     * @return
     */
    public boolean isLogcatAlive() {
        return mLogCollector != null && mLogCollector.isLogcatAlive();
    }

    /**
     * 开启日志输入至文件中
     */
    public void start() {
        if (mLogCollector == null){
            return;
        }
        logI(TAG, "LogService recordLog start");
        mLogCollector.start();
    }

    /**
     * 添加应用内多进程的对应id
     *
     * @param pid
     */
    public void addPid(String pid) {
        if (mLogCollector == null){
            return;
        }
        mLogCollector.addPid(pid);
    }

    /**
     * 写日志,在用户线程中调用
     *
     * @param content
     */
    public void writeLog(String content) {
        if (mLogCollector == null){
            return;
        }
        mLogCollector.writeLog(content);
    }

    /**
     * 销毁log进程
     */
    public void stopProcess() {
        if (mLogCollector == null){
            return;
        }
        logD(TAG, "destroy logcat Process");
        mLogCollector.stop();
    }

    /**
     * 将日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     *
     * @return 返回日志压缩文件的绝对路径
     */
    public String appendZipLog() {
        if (mLogCollector == null){
            return null;
        }
        return mLogCollector.appendZipLog();
    }

    /**
     * 播放器异常时收集相关日志
     */
    public void savePlayerLog() {
        if (mPlayerLogCollector == null) {
            mPlayerLogCollector = initPlayerCollector();
        }
        String endTag = "savePlayerLog !Time:" + System.nanoTime();
        logI(TAG, endTag);
        mPlayerLogCollector.start(endTag);
    }

    /**
     * 将播放日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     *
     * @return 返回日志压缩文件的绝对路径
     */
    public String appendPlayerZipLog() {
        if (mPlayerLogCollector == null) {
            mPlayerLogCollector = initPlayerCollector();
        }
        return mPlayerLogCollector.appendZipLog();
    }

    /**
     * 应用崩溃时，收集相关日志
     */
    public void saveCrashLog() {
        saveCrashLog(null);
    }

    public void saveCrashLog(ILogStatusCallback callback) {
        if (mCrashLogCollector == null) {
            mCrashLogCollector = initCrashCollector();
        }
        mCrashLogCollector.setLogCollectCallback(callback);
        String endTag = "saveCrashLog !Time:" + System.nanoTime();
        logI(TAG, endTag);
        mCrashLogCollector.start(endTag);
    }

    /**
     * 将崩溃日志路径下的所有日志文件排序拼接成一个压缩后的OutputStream流
     *
     * @return 返回日志压缩文件的绝对路径
     */
    public String appendCrashZipLog() {
        if (mCrashLogCollector == null) {
            mCrashLogCollector = initCrashCollector();
        }
        return mCrashLogCollector.appendZipLog();
    }

    /**
     * 将所有日志压缩到一个zip包（通用日志、播放日志、崩溃日志）
     *
     * @return
     */
    public String appendAllLog() {
        String normalLogPath = appendZipLog();
        String playerLogPath = appendPlayerZipLog();
        String crashLogPath = appendCrashZipLog();
        List<File> files = new ArrayList<>();
        if (!isEqualsNull(normalLogPath)) {
            files.add(new File(normalLogPath));
        }
        if (!isEqualsNull(playerLogPath)) {
            files.add(new File(playerLogPath));
        }
        if (!isEqualsNull(crashLogPath)) {
            files.add(new File(crashLogPath));
        }
        File zipFile;
        try {
            zipFile = new File(LogFileHelper.getLogZipPath(ALL_LOG_ZIP_FILE_PATH));
            if (zipFile.exists()) {
                // 压缩文件已存在时，删除
                zipFile.delete();
            }
            LogManager.getInstance().doZipFilesWithPassword(zipFile, null, files,
                    null, null);
            return zipFile.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    private OnceLogCollector initPlayerCollector() {
        return new OnceLogCollector(PLAYER_LOG_DIR, PLAYER_LOG_FILE_PREFIX,
                PLAYER_LOG_APPEND_FILE_NAME, PLAYER_LOG_ZIP_FILE_PATH, TAG);
    }

    private OnceLogCollector initCrashCollector() {
        return new OnceLogCollector(CRASH_LOG_DIR, CRASH_LOG_FILE_PREFIX,
                CRASH_LOG_APPEND_FILE_NAME, CRASH_LOG_ZIP_FILE_PATH, TAG);
    }

    Context getAppContext() {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getAppContext();
    }

    String getPackageName() {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getPackageName();
    }

    void killLogcatProcess() {
        if (mProxy == null) {
            return;
        }
        mProxy.killLogcatProcess();
    }

    /**
     * 初始化日志打印监听
     *
     * @param logListener
     */
    void initMGLogListener(ILogListener logListener) {
        if (mProxy == null) {
            return;
        }
        mProxy.initMGLogListener(logListener);
    }

    void logD(String tag, String msg) {
        if (mProxy == null) {
            return;
        }
        mProxy.d(tag, msg);
    }

    void logI(String tag, String msg) {
        if (mProxy == null) {
            return;
        }
        mProxy.i(tag, msg);
    }

    boolean isEqualsNull(String str) {
        if (mProxy == null) {
            return false;
        }
        return mProxy.isEqualsNull(str);
    }

    boolean isDebugOn() {
        if (mProxy == null) {
            return false;
        }
        return mProxy.isDebugOn();
    }

    String getCurrentTime(String format) {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getCurrentTime(format);
    }

    File getExternalCacheDir() {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getExternalCacheDir();
    }

    File getInternalCacheDir() {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getInternalCacheDir();
    }

    File getFileByAbsolutePath(String path) {
        if (mProxy == null) {
            return null;
        }
        return mProxy.getFileByAbsolutePath(path);
    }

    File[] orderByDate(String fileDir) {
        if (mProxy == null) {
            return null;
        }
        return mProxy.orderByDate(fileDir);
    }

    void closeIOStream(OutputStream out) {
        if (mProxy == null) {
            return;
        }
        mProxy.closeIOStream(out);
    }

    /**
     * 对文件列表压缩加密
     */
    void doZipFilesWithPassword(File zipFile, char[] password, List<File> srcFiles,
                                String appendName, File anotherFile) {
        if (mProxy == null) {
            return;
        }
        mProxy.doZipFilesWithPassword(zipFile, password, srcFiles, appendName, anotherFile);
    }

    void runInThread(Runnable runnable) {
        if (mProxy == null) {
            return;
        }
        mProxy.runInThread(runnable);
    }
}


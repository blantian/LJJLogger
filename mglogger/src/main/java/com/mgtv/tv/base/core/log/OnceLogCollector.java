package com.mgtv.tv.base.core.log;


import androidx.annotation.NonNull;

import com.mgtv.tv.base.core.log.listener.ILogStatusCallback;


/**
 * 日志收集器，快速收集当前时间点日志
 * Created by wukeqi on 2023/11/28.
 * keqi@mgtv.com
 */
public class OnceLogCollector extends BaseLogCollector {

    private static final long MAX_LOG_DIR_LENGTH =1024 * 1024;// 每个日志文件logcat大小限制为1M
    private static final int MAX_LOG_FILE_SIZE = 3;// 最多三个文件记录日志

    private String mEndTag;
    private ILogStatusCallback callback;

    public OnceLogCollector(@NonNull String logDir, @NonNull String logPrefix,
                            @NonNull String logAppendName, @NonNull String logZipPath,
                            @NonNull String tag) {
        super(logDir, logPrefix, logAppendName, logZipPath,null,
                MAX_LOG_DIR_LENGTH, MAX_LOG_FILE_SIZE, tag);
    }

    /**
     * 设置日志收集的回调
     * @param callback
     */
    public void setLogCollectCallback(ILogStatusCallback callback){
        this.callback = callback;
    }

    /**
     * 启动收集
     */
    public void start(String endTag){
      mEndTag = endTag;
      stop();
      super.start();
    }

    @Override
    protected void run() {
        if (callback != null) {
            callback.onLogCollectStart();
        }
        String[] cmds = LogManager.getCmd();
        try {
            resetBufferReader(cmds);
            String line;
            while (true) {
                line = mReader.readLine();
                if (line == null) {
                    // log进程异常退出，停止收集
                    break;
                }
                checkLogSize(line);
                // 收集所有日志
                String content = line + "\r\n";
                mFos.write(content.getBytes());
                mFos.flush();
                if (line.endsWith(mEndTag)) {
                    // 已到达停止tag，停止收集
                    break;
                }
            }
        } catch (Exception e) {
            LogManager.getInstance().logI(mTag, "recordLog failed" + e);
        } finally {
            if (callback != null) {
                callback.onLogCollectComplete();
            }
            stop();
            LogManager.getInstance().logI(mTag, "recordLog end!");
        }
    }

    @Override
    public void stop() {
        if (mThread != null){
            mThread.interrupt();
        }
        super.stop();
    }
}
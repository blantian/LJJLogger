package com.mgtv.logger.log;

import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;

public abstract class SendLogRunnable implements Runnable{

    public static final int STATUS_SENDING  = 1;
    public static final int STATUS_FINISHED = 2;
    public static final int STATUS_FAILED   = -1;
    public static final int IDLE = 0;

    private volatile SendAction sendAction;
    private volatile OnSendLogCallBackListener callback;

    public void setSendAction(@NonNull SendAction action) {
        this.sendAction = action;
    }

    public void setCallBackListener(@Nullable OnSendLogCallBackListener cb) {
        this.callback = cb;
    }

    /** 子类实现具体上传逻辑 */
    protected abstract void sendLog(@NonNull File logFile) throws Exception;

    @Override
    public final void run() {
        SendAction action = this.sendAction;
        if (action == null || TextUtils.isEmpty(action.uploadPath)) {
            finish(STATUS_FAILED);
            return;
        }

        File file = new File(action.uploadPath);
        int status = STATUS_FINISHED;
        try {
            sendLog(file);
        } catch (Exception e) {
            status = STATUS_FAILED;
            onError(e);
        } finally {
            finish(status);
            if (file.getName().endsWith(".copy")) {
                //noinspection ResultOfMethodCallIgnored
                file.delete();
            }
        }
    }

    /** 子类可重写以处理异常 */
    protected void onError(@NonNull Exception e) { }

    private void finish(int status) {
        OnSendLogCallBackListener cb = this.callback;
        if (cb != null) cb.onCallBack(status);
    }

    public interface OnSendLogCallBackListener {
        void onCallBack(int statusCode);
    }
}

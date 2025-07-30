package com.mgtv.logger.log;

import android.text.TextUtils;

class WriteAction {

    String log; //日志

    boolean isMainThread;

    long threadId;

    String threadName = "";

    long localTime;

    int flag;

    boolean isValid() {
        boolean valid = false;
        if (!TextUtils.isEmpty(log)) {
            valid = true;
        }
        return valid;
    }
}

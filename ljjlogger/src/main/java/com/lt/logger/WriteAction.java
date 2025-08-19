package com.lt.logger;

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

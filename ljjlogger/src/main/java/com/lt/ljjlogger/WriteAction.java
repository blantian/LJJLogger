package com.lt.ljjlogger;

import android.text.TextUtils;

public class WriteAction {

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

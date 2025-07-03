package com.mgtv.mglogger;

import android.util.Log;

import com.mgtv.logger.kt.log.MGLogger;

public class CrashHandler implements Thread.UncaughtExceptionHandler {

    private final Thread.UncaughtExceptionHandler defaultHandler;

    private CrashHandler() {
        this.defaultHandler = Thread.getDefaultUncaughtExceptionHandler();
    }

    public static void install() {
        Thread.setDefaultUncaughtExceptionHandler(new CrashHandler());
    }

    @Override
    public void uncaughtException(Thread t, Throwable e) {
        MGLogger.w(Log.getStackTraceString(e), 3);
        MGLogger.flush();
        if (defaultHandler != null) {
            defaultHandler.uncaughtException(t, e);
        }
    }
}

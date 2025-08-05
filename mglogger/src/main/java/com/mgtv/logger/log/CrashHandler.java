package com.mgtv.logger.log;

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
        if (defaultHandler != null) {
            defaultHandler.uncaughtException(t, e);
        }
    }
}

package com.lt.ljjlogger.i;

public interface ILoggerStatus {
    /**
     * 日志状态回调
     *
     * @param code 状态码
     * @param msg  状态信息
     */
    void loggerStatus(int code, String msg);
}

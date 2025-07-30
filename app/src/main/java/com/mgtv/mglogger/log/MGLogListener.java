package com.mgtv.mglogger.log;

/**
 * @author Zhouguang on 2017/11/8
 * @description 日志接口类
 * @Email zhouguang@mgtv.com
 */

public interface MGLogListener {

    /**
     * log Verbos打印
     *
     * @param tag
     * @param msg
     * @param tr
     */
    void v(String tag, String msg, Throwable tr);

    /**
     * log Debug打印
     *
     * @param tag
     * @param msg
     * @param tr
     */
    void d(String tag, String msg, Throwable tr);

    /**
     * log Info打印
     *
     * @param tag
     * @param msg
     * @param tr
     */
    void i(String tag, String msg, Throwable tr);

    /**
     * log Warn打印
     *
     * @param tag
     * @param msg
     * @param tr
     */
    void w(String tag, String msg, Throwable tr);

    /**
     * log Warn打印
     *
     * @param tag
     * @param throwable
     */
    void w(String tag, Throwable throwable);

    /**
     * log Error打印
     *
     * @param tag
     * @param throwable
     */
    void e(String tag, Throwable throwable);

    /**
     * log Error打印
     *
     * @param tag
     * @param msg
     * @param tr
     */
    void e(String tag, String msg, Throwable tr);
}

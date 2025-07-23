package com.mgtv.mglogger.log;

import static android.util.Log.getStackTraceString;

import android.util.Log;

import com.mgtv.mglogger.log.utils.StringUtils;
import com.mgtv.mglogger.log.utils.TimeUtils;

/**
 * @author Zhouguang on 2017/10/23
 * @description 日志打印通用类
 * @Email zhouguang@mgtv.com
 */
public class MGLog {
    private static final String VALUE_ON = "1";
    private static final String VALUE_OFF = "0";
    private static final String DEFAULT_TAG = "mgtv-ott";
    private static MGLogListener mLogListener;
    public static boolean sIsDebug;//是否为debug版本
    private static final String KEY_OPEN_DEBUG_LOG = "basecore_openDebugLog";
    private static final String TIME_TEMPLATE = "yyyy-MM-dd HH:mm:ss.SSS";

    /**
     * 视达科外部调用，初始化日志打印监听
     *
     * @param logListener
     */
    public static void initMGLogListener(MGLogListener logListener) {
        mLogListener = logListener;
    }

    public static void init(boolean isDebug) {
        String sharePreferIsDebug = SharedPreferenceUtils.getString(null, KEY_OPEN_DEBUG_LOG, null);
        if (VALUE_ON.equals(sharePreferIsDebug)) {
            sIsDebug = true;
        } else if (VALUE_OFF.equals(sharePreferIsDebug)) {
            sIsDebug = false;
        } else {
            sIsDebug = isDebug;
        }
    }

    public static void i(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        i(tag, msg, null);
    }

    public static void e(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        e(tag, msg, null);
    }

    public static void v(String tag, String msg) {

        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        v(tag, msg, null);
    }

    public static void d(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        d(tag, msg, null);
    }

    public static void w(String tag, String msg) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        w(tag, msg, null);
    }

    public static void i(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.i(tag, msg, tr);
            } else {
                Log.i(tag, msg, tr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void e(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.e(tag, msg, tr);
            } else {
                Log.e(tag, msg, tr);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void v(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.v(tag, msg, tr);
            } else {
                // debug版本才能输出verbose级别日志
                if (sIsDebug) {
                    Log.v(tag, msg, tr);
                    String time = TimeUtils.transformToString(TimeUtils.getCurrentTime(), TIME_TEMPLATE);
                    LogManager.getInstance().bufferLog(time + " V/" + tag + ": " + msg + '\n' + getStackTraceString(tr));
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void d(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.d(tag, msg, tr);
            } else {
                // debug版本才能输出debug级别日志
                if (sIsDebug) {
                    Log.d(tag, msg, tr);
                    String time = TimeUtils.transformToString(TimeUtils.getCurrentTime(), TIME_TEMPLATE);
                    LogManager.getInstance().bufferLog(time + " D/" + tag + ": " + msg + '\n' + getStackTraceString(tr));
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void w(String tag, String msg, Throwable tr) {
        if (StringUtils.equalsNull(tag) || StringUtils.equalsNull(msg)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.w(tag, msg, tr);
            } else {
                Log.w(tag, msg, tr);
                String time = TimeUtils.transformToString(TimeUtils.getCurrentTime(), TIME_TEMPLATE);
                LogManager.getInstance().bufferLog(time + " W/" + tag + ": " + msg + '\n' + getStackTraceString(tr));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void w(String tag, Throwable throwable) {
        if (StringUtils.equalsNull(tag)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.w(tag, throwable);
            } else {
                Log.w(tag, throwable);
                String time = TimeUtils.transformToString(TimeUtils.getCurrentTime(), TIME_TEMPLATE);
                LogManager.getInstance().bufferLog(time + " W/" + tag + ": " + getStackTraceString(throwable));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void e(String tag, Throwable throwable) {
        if (StringUtils.equalsNull(tag)) {
            return;
        }
        try {
            if (mLogListener != null) {
                mLogListener.e(tag, throwable);
            } else {
                Log.e(tag, "", throwable);
                String time = TimeUtils.transformToString(TimeUtils.getCurrentTime(), TIME_TEMPLATE);
                LogManager.getInstance().bufferLog(time + " E/" + tag + ": " + getStackTraceString(throwable));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void i(String msg) {
        i(DEFAULT_TAG, msg);
    }

    public static void e(String msg) {
        e(DEFAULT_TAG, msg);
    }

    public static void v(String msg) {
        v(DEFAULT_TAG, msg);
    }

    public static void d(String msg) {
        d(DEFAULT_TAG, msg);
    }

    public static void w(String msg) {
        w(DEFAULT_TAG, msg);
    }

    /**
     * 设置日志打印级别，0release 1debug
     *
     * @param isDebug
     */
    public static void setMglogIsDebug(String isDebug) {
        if (VALUE_ON.equals(isDebug)) {
            sIsDebug = true;
        } else if (VALUE_OFF.equals(isDebug)) {
            sIsDebug = false;
        }
        MGLog.i("--->setStringValue value:" + isDebug + ",key:" + KEY_OPEN_DEBUG_LOG);
        SharedPreferenceUtils.put(null, KEY_OPEN_DEBUG_LOG, isDebug);
    }

}
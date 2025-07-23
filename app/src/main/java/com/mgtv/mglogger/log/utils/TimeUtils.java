package com.mgtv.mglogger.log.utils;



import androidx.annotation.Keep;

import com.mgtv.mglogger.log.MGLog;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;

/**
 * description:
 * <p>
 * author: Created by xianggengping on 2017/11/7.
 * <p>
 * email: gengping@mgtv.com
 */

@Keep
public final class TimeUtils {


    private static final String TAG = "TimeUtils";
    private static ITimeHandler sTimeHandler;
    private static final String DEFAULT_FORMAT_MILLIS = "yyyy-MM-dd HH:mm:ss.SSS";
    private static final String DEFAULT_FORMAT_MILLIS2 = "yyyy.MM.dd HH:mm:ss.SSS";
    private static final String DEFAULT_FORMAT_SECOND = "yyyy-MM-dd HH:mm:ss";
    private static final String DEFAULT_FORMAT_SECOND2 = "yyyy.MM.dd HH:mm:ss";
    private static final String DEFAULT_FORMAT_SECOND3 = "yyyy-MM-dd HH-mm-ss";
    private static final String DEFAULT_FORMAT_MINUTE = "yyyy-MM-dd HH:mm";
    private static final String DEFAULT_FORMAT_MINUTE2 = "yyyy.MM.dd HH:mm";

    /**
     * 线程独享 Calendar，避免同步开销 & GC 抖动
     */
    private static final ThreadLocal<Calendar> FAST_CAL =
            new ThreadLocal<Calendar>() {
                @Override
                protected Calendar initialValue() {
                    return Calendar.getInstance(); // 默认时区 & Locale
                }
            };


    /* =============== 内部工具 =============== */

    /**
     * <br/>快速格式化时间
     *
     * @param millis 毫秒值
     * @param format 时间格式
     * @return 返回格式化后的时间字符串
     */
    private static String fastFormat(long millis, String format) {
        Calendar c = FAST_CAL.get();
        if (c == null) {
            c = Calendar.getInstance();
            FAST_CAL.set(c);
        }

        c.setTimeInMillis(millis);
        int y = c.get(Calendar.YEAR);
        int mon = c.get(Calendar.MONTH) + 1;          // Calendar 月份从 0 开始
        int d = c.get(Calendar.DAY_OF_MONTH);
        int h = c.get(Calendar.HOUR_OF_DAY);
        int m = c.get(Calendar.MINUTE);
        int s = c.get(Calendar.SECOND);
        int ms = c.get(Calendar.MILLISECOND);

        StringBuilder sb = new StringBuilder(23);
        // 根据格式选择输出
        switch (format) {
            case DEFAULT_FORMAT_MILLIS:
                append4(sb, y).append('-');
                append2(sb, mon).append('-');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m).append(':');
                append2(sb, s).append('.');
                append3(sb, ms);
                break;
            case DEFAULT_FORMAT_MILLIS2:
                append4(sb, y).append('.');
                append2(sb, mon).append('.');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m).append(':');
                append2(sb, s).append('.');
                append3(sb, ms);
                break;
            case DEFAULT_FORMAT_SECOND:
                append4(sb, y).append('-');
                append2(sb, mon).append('-');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m).append(':');
                append2(sb, s);
                break;
            case DEFAULT_FORMAT_SECOND2:
                append4(sb, y).append('.');
                append2(sb, mon).append('.');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m).append(':');
                append2(sb, s);
                break;
            case DEFAULT_FORMAT_SECOND3:
                append4(sb, y).append('-');
                append2(sb, mon).append('-');
                append2(sb, d).append(' ');
                append2(sb, h).append('-');
                append2(sb, m).append('-');
                append2(sb, s);
                break;
            case DEFAULT_FORMAT_MINUTE:
                append4(sb, y).append('-');
                append2(sb, mon).append('-');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m);
                break;
            case DEFAULT_FORMAT_MINUTE2:
                append4(sb, y).append('.');
                append2(sb, mon).append('.');
                append2(sb, d).append(' ');
                append2(sb, h).append(':');
                append2(sb, m);
                break;
            default:
                MGLog.w(TAG, "Unsupported data format: " + format);
        }
        return sb.toString();
    }

    private static StringBuilder append2(StringBuilder sb, int v) {
        if (v < 10) sb.append('0');
        return sb.append(v);
    }

    private static StringBuilder append3(StringBuilder sb, int v) {
        if (v < 100) sb.append('0');
        if (v < 10) sb.append('0');
        return sb.append(v);
    }

    private static StringBuilder append4(StringBuilder sb, int v) {
        if (v < 1000) sb.append('0');
        if (v < 100) sb.append('0');
        if (v < 10) sb.append('0');
        return sb.append(v);
    }

    public static long getSystemCurrentTime() {
        return System.currentTimeMillis();
    }

    /**
     * <br/>设置时间处理器
     *
     * @param timeHandler
     */
    public static void setTimeHandler(ITimeHandler timeHandler) {
        sTimeHandler = timeHandler;
    }

    /**
     * <br/>获取与服务端同步后的时间，当获取失败的返回为系统当前时间
     *
     * @return
     */
    public static long getCurrentTime() {
        if (sTimeHandler != null) {
            return sTimeHandler.getCurrentTime();
        }
        return getSystemCurrentTime();
    }

    /**
     * <br/>字符串转换成时间
     *
     * @param str      时间字符串,例如：2017-11-07 12:12:12
     * @param template 时间字符串模板，例如：yyyy-MM-dd HH:mm:ss
     * @return 返回 {@link Date} 类对象的实例
     */
    public static Date transformToDate(String str, String template) {
        if (StringUtils.equalsNull(str) || StringUtils.equalsNull(template)) {
            return null;
        }
        SimpleDateFormat df = new SimpleDateFormat(template,
                Locale.getDefault());
        Date date = null;
        try {
            date = df.parse(str);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return date;
    }

    /**
     * <br/>时间转换成字符串
     *
     * @param timeMills 当前时间的毫秒值
     * @param template  需要转换成字符串的模板，例如：yyyy-MM-dd HH:mm:ss
     * @return 返回转换后的字符串
     */
    public static String transformToString(long timeMills, String template) {
        if (StringUtils.equalsNull(template)) {
            return null;
        }
        return fastFormat(timeMills, template);
    }


    /**
     * <br/>获取准确时间的处理接口
     */
    public interface ITimeHandler {

        /**
         * <br/>获取与服务端同步的时间
         *
         * @return
         */
        long getCurrentTime();
    }
}


package com.lt.logger;

import java.text.SimpleDateFormat;
import java.util.Date;

public class LoggerUtils {
    private static final String TAG = "LoggerUtils";
    private static final SimpleDateFormat sDateFormat = new SimpleDateFormat("yyyy-MM-dd");

    /**
     * 获取当前时间的毫秒数，格式化为 yyyy-MM-dd 的字符串后再转换为毫秒数。
     *
     * @return 当前时间的毫秒数
     */
    public static long getCurrentTime() {
        long currentTime = System.currentTimeMillis();
        long tempTime = 0;
        try {
            String dataStr = sDateFormat.format(new Date(currentTime));
            tempTime = sDateFormat.parse(dataStr).getTime();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return tempTime;
    }
}

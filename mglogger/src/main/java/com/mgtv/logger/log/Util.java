
package com.mgtv.logger.log;

import java.text.SimpleDateFormat;
import java.util.Date;

public class Util {

    private static final SimpleDateFormat sDateFormat = new SimpleDateFormat("yyyy-MM-dd");

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

    public static String getDateStr(long time) {
        return sDateFormat.format(new Date(time));
    }
}

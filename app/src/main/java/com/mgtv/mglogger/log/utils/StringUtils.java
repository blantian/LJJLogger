package com.mgtv.mglogger.log.utils;

public class StringUtils {

    public static boolean equalsNull(String str) {

        return isBlank(str) || str.equalsIgnoreCase("null");

    }


    public static boolean isBlank(String str) {
        int strLen;
        if (str == null || (strLen = str.length()) == 0) {
            return true;
        }
        for (int i = 0; i < strLen; i++) {
            if ((!Character.isWhitespace(str.charAt(i)))) {
                return false;
            }
        }
        return true;
    }
}

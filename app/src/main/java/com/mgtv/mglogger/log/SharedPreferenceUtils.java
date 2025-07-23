package com.mgtv.mglogger.log;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.core.content.SharedPreferencesCompat;

import com.mgtv.mglogger.log.utils.StringUtils;
import com.mgtv.mglogger.log.utils.ContextProvider;

import java.util.Map;

/**
 * description: SharedPreference工具类，实现了SharedPreference的基本操作
 * <p>
 * author: Created by xianggengping on 2017/10/31.
 * <p>
 * email: gengping@mgtv.com
 */

public final class SharedPreferenceUtils {
    private static final String DEFAULT_SP_NAME = "mgtv";
    private static final String SP_DIRECTORY = "com_mgtv_tv_sharedPreference";

    /**
     * 存放基本数据类型到sharedPreference中
     * @param fileName
     *            内容存放的xml文件名
     * @param key
     *            存放内容的key值
     * @param value
     *            存放value值
     */
    public static void put(String fileName, String key, Object value) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        SharedPreferences sharedPreferences = ContextProvider
                .getApplicationContext().getSharedPreferences(
                        getFilePath(fileName), Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        if (value instanceof Integer) {
            editor.putInt(key, (Integer) value);
        } else if (value instanceof Long) {
            editor.putLong(key, (Long) value);
        } else if (value instanceof Float) {
            editor.putFloat(key, (Float) value);
        } else if (value instanceof String) {
            editor.putString(key, (String) value);
        } else if (value instanceof Boolean) {
            editor.putBoolean(key, (Boolean) value);
        }
        SharedPreferencesCompat.EditorCompat.getInstance().apply(editor);
    }

    /**
     * 从sharedPreference中获取指定key的int值
     * @param fileName
     *            指定从哪个文件中获取
     * @param key
     *            关键字
     * @param defaultValue
     *            默认值
     * @return
     */
    public static int getInt(String fileName, String key, int defaultValue) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        return ContextProvider.getApplicationContext()
                .getSharedPreferences(getFilePath(fileName),
                        Context.MODE_PRIVATE)
                .getInt(key, defaultValue);
    }

    /**
     * 从sharedPreference中获取指定key的布尔值
     * @param fileName
     *            指定从哪个文件中获取
     * @param key
     *            关键字
     * @param defaultValue
     *            默认值
     * @return
     */
    public static boolean getBoolean(String fileName, String key,
            boolean defaultValue) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        return ContextProvider.getApplicationContext()
                .getSharedPreferences(getFilePath(fileName),
                        Context.MODE_PRIVATE)
                .getBoolean(key, defaultValue);
    }

    /**
     * 从sharedPreference中获取指定key的Long值
     * @param fileName
     *            指定从哪个文件中获取
     * @param key
     *            关键字
     * @param defaultValue
     *            默认值
     * @return
     */
    public static long getLong(String fileName, String key, long defaultValue) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        return ContextProvider.getApplicationContext()
                .getSharedPreferences(getFilePath(fileName),
                        Context.MODE_PRIVATE)
                .getLong(key, defaultValue);
    }

    /**
     * 从sharedPreference中获取指定key的String值
     * @param fileName
     *            指定从哪个文件中获取
     * @param key
     *            关键字
     * @param defaultValue
     *            默认值
     * @return
     */
    public static String getString(String fileName, String key,
            String defaultValue) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        return ContextProvider.getApplicationContext()
                .getSharedPreferences(getFilePath(fileName),
                        Context.MODE_PRIVATE)
                .getString(key, defaultValue);
    }

    /**
     * 从sharedPreference中获取指定key的float值
     * @param fileName
     *            指定从哪个文件中获取
     * @param key
     *            关键字
     * @param defaultValue
     *            默认值
     * @return
     */
    public static float getFloat(String fileName, String key,
            float defaultValue) {
        if (StringUtils.equalsNull(fileName)) {
            fileName = DEFAULT_SP_NAME;
        }
        return ContextProvider.getApplicationContext()
                .getSharedPreferences(getFilePath(fileName),
                        Context.MODE_PRIVATE)
                .getFloat(key, defaultValue);
    }

    /**
     * 清空sharedPreference指定文件
     *
     * @param fileName 指定xml文件名
     * @return
     */
    public static boolean clearFile(String fileName) {
        SharedPreferences sharedPreferences = ContextProvider
                .getApplicationContext().getSharedPreferences(
                        getFilePath(fileName), Context.MODE_PRIVATE);
        return sharedPreferences != null && sharedPreferences.edit().clear().commit();
    }

    /**
     * 获取sharedPreference指定文件存储的所有内容
     *
     * @param fileName 指定xml文件名
     * @return
     */
    public static Map<String, ?> getAll(String fileName) {
        SharedPreferences sharedPreferences = ContextProvider
                .getApplicationContext().getSharedPreferences(
                        getFilePath(fileName), Context.MODE_PRIVATE);
        if (sharedPreferences != null) {
            return sharedPreferences.getAll();
        }
        return null;
    }

    private static String getFilePath(String fileName) {
        return SP_DIRECTORY + fileName;
    }
}

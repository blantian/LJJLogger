package com.mgtv.logger.kt.common

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 10:50
 */
public object MGLoggerStatus {
    public const val MGLOGGER_INIT_STATUS: String = "mglooger_init" //初始化函数
    public val MGLOGGER_INIT_SUCCESS_MMAP: Int = -1010 //初始化成功, mmap内存
    public val MGLOGGER_INIT_SUCCESS_MEMORY: Int = -1020 //初始化成功, 堆内存
    public val MGLOGGER_INIT_FAIL_NOCACHE: Int = -1030 //初始化失败 , 没有缓存
    public val MGLOGGER_INIT_FAIL_NOMALLOC: Int = -1040 //初始化失败 , malloc失败
    public val MGLOGGER_INIT_FAIL_HEADER: Int = -1050 //初始化头失败
    public val MGLOGGER_INIT_FAIL_JNI: Int = -1060 //jni找不到对应C函数

    public const val MGLOGGER_OPEN_STATUS: String = "clogan_open" //打开文件函数
    public val MGLOGGER_OPEN_SUCCESS: Int = -2010 //打开文件成功
    public val MGLOGGER_OPEN_FAIL_IO: Int = -2020 //打开文件IO失败
    public val MGLOGGER_OPEN_FAIL_ZLIB: Int = -2030 //打开文件zlib失败
    public val MGLOGGER_OPEN_FAIL_MALLOC: Int = -2040 //打开文件malloc失败
    public val MGLOGGER_OPEN_FAIL_NOINIT: Int = -2050 //打开文件没有初始化失败
    public val MGLOGGER_OPEN_FAIL_HEADER: Int = -2060 //打开文件头失败
    public val MGLOGGER_OPEN_FAIL_JNI: Int = -2070 //jni找不到对应C函数

    public const val MGLOGGER_WRITE_STATUS: String = "clogan_write" //写入函数
    public val MGLOGGER_WRITE_SUCCESS: Int = -4010 //写入日志成功
    public val MGLOGGER_WRITE_FAIL_PARAM: Int = -4020 //写入失败, 可变参数错误
    public val MGLOGGER_WRITE_FAIL_MAXFILE: Int = -4030 //写入失败,超过文件最大值
    public val MGLOGGER_WRITE_FAIL_MALLOC: Int = -4040 //写入失败,malloc失败
    public val MGLOGGER_WRITE_FAIL_HEADER: Int = -4050 //写入头失败
    public val MGLOGGER_WRITE_FAIL_JNI: Int = -4060 //jni找不到对应C函数
    public const val MGLOGGER_LOAD_SO: String = "logan_loadso" //Logan装载So;
    public val MGLOGGER_LOAD_SO_FAIL: Int = -5020 //加载的SO失败

    public const val MGLOGGER_LOGCAT_COLLECTOR_STATUS: String = "logcat_collector"
    public val MGLOGGER_LOGCAT_COLLECTOR_FAIL: Int = -6010
}
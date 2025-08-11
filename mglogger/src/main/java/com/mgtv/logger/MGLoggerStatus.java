package com.mgtv.logger;
/**
 * Description:
 * Created by lantian
 * Date： 2025/7/31
 * Time： 11:21
 */
public final class MGLoggerStatus {

    private MGLoggerStatus() {
        throw new AssertionError("No MGLoggerStatus instances.");
    }

    /* --------------------- 初始化 --------------------- */
    public static final String MGLOGGER_INIT_STATUS        = "logger_init"; // 初始化函数
    public static final int    MGLOGGER_INIT_SUCCESS_MMAP  = 1010; // 初始化成功, mmap内存
    public static final int    MGLOGGER_INIT_SUCCESS_MEMORY= 1020; // 初始化成功, 堆内存
    public static final int    MGLOGGER_INIT_FAIL_NOCACHE  = 1030; // 初始化失败 , 没有缓存
    public static final int    MGLOGGER_INIT_FAIL_NOMALLOC = 1040; // 初始化失败 , malloc失败
    public static final int    MGLOGGER_INIT_FAIL_HEADER   = 1050; // 初始化头失败
    public static final int    MGLOGGER_INIT_FAIL_JNI      = 1060; // jni找不到对应C函数

    /* --------------------- 打开文件 --------------------- */
    public static final String MGLOGGER_OPEN_STATUS        = "logger_open"; // 打开文件函数
    public static final int    MGLOGGER_OPEN_SUCCESS       = 2010; // 打开文件成功
    public static final int    MGLOGGER_OPEN_FAIL_IO       = 2020; // 打开文件IO失败
    public static final int    MGLOGGER_OPEN_FAIL_ZLIB     = 2030; // 打开文件zlib失败
    public static final int    MGLOGGER_OPEN_FAIL_MALLOC   = 2040; // 打开文件malloc失败
    public static final int    MGLOGGER_OPEN_FAIL_NOINIT   = 2050; // 打开文件没有初始化失败
    public static final int    MGLOGGER_OPEN_FAIL_HEADER   = 2060; // 打开文件头失败
    public static final int    MGLOGGER_OPEN_FAIL_JNI      = 2070; // jni找不到对应C函数

    /* --------------------- 写入日志 --------------------- */
    public static final String MGLOGGER_WRITE_STATUS       = "logger_write"; // 写入函数
    public static final int    MGLOGGER_WRITE_SUCCESS      = 4010; // 写入日志成功
    public static final int    MGLOGGER_WRITE_FAIL_PARAM   = 4020; // 写入失败, 可变参数错误
    public static final int    MGLOGGER_WRITE_FAIL_MAXFILE = 4030; // 写入失败,超过文件最大值
    public static final int    MGLOGGER_WRITE_FAIL_MALLOC  = 4040; // 写入失败,malloc失败
    public static final int    MGLOGGER_WRITE_FAIL_HEADER  = 4050; // 写入头失败
    public static final int    MGLOGGER_WRITE_FAIL_JNI     = 4060; // jni找不到对应C函数

    /* --------------------- SO 加载 --------------------- */
    public static final String MGLOGGER_LOAD_SO            = "logger_load_so"; // Logan装载So
    public static final int    MGLOGGER_LOAD_SO_FAIL       = 5020;          // 加载的SO失败

    /* --------------------- Logcat Collector ------------- */
    public static final String MGLOGGER_LOGCAT_COLLECTOR_STATUS  = "logcat_collector";
    public static final int    MGLOGGER_LOGCAT_COLLECTOR_SUCCESS = 6000;
    public static final int    MGLOGGER_LOGCAT_COLLECTOR_FAIL    = 6010;

    /* --------------------- 其他 --------------------- */
    public static final String MGLOGGER_OTHER_STATUS       = "logger_other"; // 其他函数
    public static final int    MGLOGGER_OK         = 200;
    public static final int    MGLOGGER_ERROR      = -1; // 错误
    public static final int    MGLOGGER_CREATE_FAILED          = 400; // 创建失败
    public static final int    MGLOGGER_CREATE_QUEUE_FAILED    = 410; // 创建队列失败
    public static final int    MGLOGGER_MESSAGE_QUEUE_FAILED   = 420; // 消息队列失败
    public static final int    MGLOGGER_FORK_THREAD_FAILED     = 430; // 创建线程失败
    public static final int    MGLOGGER_HOOK_FAILED            = 440; // hook失败
    public static final int    MGLOGGER_CREATE_WORKER_THREAD_FAILED    = 450; // 创建日志处理线程失败
    public static final int    MGLOGGER_CREATE_MESSAGE_THREAD_FAILED   = 460; // 创建消息处理线程失败
    public static final int    MGLOGGER_FORK_TIMEOUT           = 470; // logcat fork超时
    public static final int    MG_LOGGER_STATUS_THREAD_CREATION_FAILED            = 1001; // logcat fork退出
    public static final int    MG_LOGGER_STATUS_PIPE_CREATION_FAILED             = 1002; // 管道创建失败
    public static final int    MG_LOGGER_STATUS_PIPE_OPEN_FAILED                 = 1003; // 管道打开失败
    public static final int    MG_LOGGER_STATUS_FORK_FAILED                      = 1004; // fork失败
    public static final int    MG_LOGGER_STATUS_FORK_EXITED                      = 1005; // fork退出

    public static final int    MG_LOGGER_STATUS_FORK_STARTED                     = 1006; // fork开始
    public static final int    MG_LOGGER_STATUS_LOGCAT_UNAVAILABLE                = 1007; // logcat不可用
    public static final int    MG_LOGGER_STATUS_FORK_TIMEOUT                      = 1008; // fork超时


}
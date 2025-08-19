package com.lt.logger.i;

import java.util.List;

public interface ILogger {

    /**
     * @param cachePath        日志缓存路径
     * @param dirPath          日志存储路径
     * @param logCacheSelector 日志采集模式：0 hook，1 logcat
     * @param maxFile          单个日志文件最大容量
     * @param maxSdCardSize    日志总容量限制，单位MB
     * @param encryptKey16     密匙，16字节
     * @param encryptIv16      IV，16字节
     */
    void loggerInit(String cachePath,
                    String dirPath,
                    int logCacheSelector,
                    int maxFile,
                    int maxSdCardSize,
                    String encryptKey16,
                    String encryptIv16);

    /**
     * 日志写入
     *
     * @param flag       日志类型
     * @param log        日志内容
     * @param localTime  日志本地时间戳
     * @param threadName 线程名称
     * @param threadId   线程ID
     * @param isMain     是否主线程
     */
    void loggerWrite(int flag,
                     String log,
                     long localTime,
                     String threadName,
                     long threadId,
                     boolean isMain);

    /**
     * 创建日志文件
     *
     * @param fileName 文件名
     */
    void loggerOpen(String fileName);


    /**
     * mmap 缓存刷入磁盘
     */
    void loggerFlush();

    /**
     * @param debug 调试
     */
    void loggerDebug(boolean debug);


    /**
     * 黑名单
     *
     * @param blackList 黑名单
     */
    void setBlackList(List<String> blackList);

    /**
     * 获取日志目录
     */
    int exportLog(String fileName);

    /**
     * 设置日志状态监听器
     *
     * @param listener 回调
     */
    void setOnLoggerStatus(ILoggerStatus listener);
}

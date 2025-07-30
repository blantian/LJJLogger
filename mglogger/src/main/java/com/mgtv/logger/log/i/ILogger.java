package com.mgtv.logger.log.i;

import java.util.List;

public interface ILogger {

    void loggerInit(String cachePath,
                    String dirPath,
                    int    logCacheSelector,
                    int    maxFile,
                    int    maxSdCardSize,
                    String encryptKey16,
                    String encryptIv16);

    void loggerWrite(int    flag,
                     String log,
                     long   localTime,
                     String threadName,
                     long   threadId,
                     boolean isMain);

    void loggerOpen(String fileName);


    void loggerFlush();

    void loggerDebug(boolean debug);


    void setBlackList(List<String> blackList);


    void setOnLoggerStatus(ILoggerStatus listener);
}

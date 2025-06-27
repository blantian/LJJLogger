package com.mgtv.logger.kt.log

import java.io.File

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 15:10
 */
public interface SendLogStrategy {
    /**
     * @return Pair<HTTP StatusCode, ResponseBody?>
     */
    public suspend fun send(logFile: File): Pair<Int, ByteArray?>
}
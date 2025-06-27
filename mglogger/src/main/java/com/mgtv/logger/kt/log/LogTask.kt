package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.i.ISendLogCallback
/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:21
 */

sealed class LogTask {
    data class Write(val log: String, val type: Int) : LogTask()
    object Flush : LogTask()
    data class Send(
        val date: String,
        val strategy: SendLogStrategy,
        val callback: ISendLogCallback?
    ) : LogTask()
}
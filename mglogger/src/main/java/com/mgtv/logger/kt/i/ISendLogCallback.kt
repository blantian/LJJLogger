package com.mgtv.logger.kt.i

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/25
 * Time： 18:55
 */
interface ISendLogCallback {
    /**
     * 日志上传结果回调方法.
     *
     * @param statusCode 对应http状态码.
     * @param data       http返回的data.
     */
    fun onLogSendCompleted(statusCode: Int, data: ByteArray?)
}
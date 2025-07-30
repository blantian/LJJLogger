package com.mgtv.logger.kt.i

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/3
 * Time： 18:47
 */
public interface ILoggerStatus {

    /**
     * Logs the status of the logger command execution.
     * @param cmd The command that was executed (e.g., "init", "write", "flush").
     * @param code The status code returned by the command execution.
     */
    public fun loggerStatus(cmd: String, code: Int)
}
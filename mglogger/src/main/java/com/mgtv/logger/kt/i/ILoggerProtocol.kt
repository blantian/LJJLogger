package com.mgtv.logger.kt.i

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/3
 * Time： 18:44
 */
public interface ILoggerProtocol {

    /**
     * Flush the logger cache to disk.
     */
    public fun logger_flush()

    /**
     * Write a log entry to the logger.
     * @param flag The log level or type (e.g., INFO, ERROR).
     * @param log The log message to write. If null, no log will be written.
     * @param local_time The local time when the log was created, in milliseconds since epoch.
     * @param thread_name The name of the thread that generated the log. If null, it will be ignored.
     * @param thread_id The ID of the thread that generated the log. If 0, it will be ignored.
     * @param is_main Indicates whether the log was generated from the main thread. If true, it will be treated as a main thread log.
     * If false, it will be treated as a background thread log.
     */
    public fun logger_write(
        flag: Int,
        log: String?,
        local_time: Long,
        thread_name: String?,
        thread_id: Long,
        is_main: Boolean
    )

    /**
     * Open a logger file with the specified name.
     * @param file_name The name of the log file to open. If null, a default file will be used.
     */
    public fun logger_open(file_name: String?)

    /**
     * Initialize the logger with the specified parameters.
     * @param cache_path The path to the cache directory.
     * @param dir_path The path to the directory where logs will be stored.
     * @param max_file The maximum number of log files to keep.
     * @param encrypt_key_16 The encryption key (16 characters).
     * @param encrypt_iv_16 The initialization vector for encryption (16 characters).
     */
    public fun logger_init(
        cache_path: String?,
        dir_path: String?,
        logCacheSelector: Int,
        max_file: Int,
        encrypt_key_16: String?,
        encrypt_iv_16: String?
    )

    /**
     * Set the debug mode for the logger.
     * @param debug If true, enables debug mode; otherwise, disables it.
     */
    public fun logger_debug(debug: Boolean)

    /**
     * Set a listener for logger status updates.
     * @param listener The listener to be notified of logger status changes.
     */
    public fun setOnLoggerStatus(listener: ILoggerStatus)

    /**
     * Update logger blacklist.
     * @param blackList Tags that should be ignored when capturing logcat.
     */
    public fun setBlackList(blackList: List<String>)
}
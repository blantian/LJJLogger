package com.mgtv.logger.kt.log

import androidx.annotation.Keep
import com.mgtv.logger.kt.common.MGLoggerStatus
import com.mgtv.logger.kt.i.ILoggerProtocol
import com.mgtv.logger.kt.i.ILoggerStatus
import java.util.Collections

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/3
 * Time： 09:20
 */
public object MGLoggerJni : ILoggerProtocol {

    @Volatile
    private var isMGLoggerOk = false
        private set

    init {
        try {
            System.loadLibrary("mglogger")
            isMGLoggerOk = true
        } catch (t: Throwable) {
            t.printStackTrace()
            isMGLoggerOk = false
        }
    }

    public fun isMGLoggerSuccess(): Boolean = isMGLoggerOk

    public fun newInstance(): MGLoggerJni = this

    private var isLoganInit = false
    private var isLoganOpen = false

    private var loggerStatus: ILoggerStatus? = null

    private val errorSet: MutableSet<Int> = Collections.synchronizedSet(HashSet())

    // ----------------------------
    // Native interface (JNI)
    // ----------------------------

    private external fun LoggerInit(
        cachePath: String?,
        dirPath: String?,
        logCacheSelector: Int,
        maxFile: Int,
        encryptKey16: String?,
        encryptIv16: String?
    ): Int

    private external fun LoggerOpen(fileName: String?): Int
    private external fun LoggerDebug(isDebug: Boolean)
    private external fun LoggerWrite(
        flag: Int,
        log: String?,
        localTime: Long,
        threadName: String?,
        threadId: Long,
        isMain: Int
    ): Int

    private external fun LoggerFlush()

    // ----------------------------
    // LoganProtocolHandler impl
    // ----------------------------

    public override fun logger_init(
        cachePath: String?,
        dirPath: String?,
        logCacheSelector: Int,
        maxFile: Int,
        encryptKey16: String?,
        encryptIv16: String?
    ) {
        if (isLoganInit) return
        if (!isMGLoggerOk) {
            loggerStatusCode(
                MGLoggerStatus.MGLOGGER_LOAD_SO,
                MGLoggerStatus.MGLOGGER_LOAD_SO_FAIL
            )
            return
        }

        try {
            val code = LoggerInit(cachePath, dirPath,logCacheSelector, maxFile, encryptKey16, encryptIv16)
            isLoganInit = true
            loggerStatusCode(MGLoggerStatus.MGLOGGER_INIT_STATUS, code)
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
            loggerStatusCode(
                MGLoggerStatus.MGLOGGER_INIT_STATUS,
                MGLoggerStatus.MGLOGGER_INIT_FAIL_JNI
            )
        }
    }

    public override fun logger_debug(debug: Boolean) {
        if (!isLoganInit || !isMGLoggerOk) return
        try {
            LoggerDebug(debug)
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
        }
    }

    public override fun setOnLoggerStatus(listener: ILoggerStatus) {
        loggerStatus = listener
    }

    public override fun logger_open(fileName: String?) {
        if (!isLoganInit || !isMGLoggerOk) return
        try {
            val code = LoggerOpen(fileName)
            isLoganOpen = true
            loggerStatusCode(MGLoggerStatus.MGLOGGER_OPEN_STATUS, code)
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
            loggerStatusCode(
                MGLoggerStatus.MGLOGGER_OPEN_STATUS,
                MGLoggerStatus.MGLOGGER_OPEN_FAIL_JNI
            )
        }
    }

    public override fun logger_flush() {
        if (!isLoganOpen || !isMGLoggerOk) return
        try {
            LoggerFlush()
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
        }
    }

    public override fun logger_write(
        flag: Int,
        log: String?,
        localTime: Long,
        threadName: String?,
        threadId: Long,
        isMain: Boolean
    ) {
        if (!isLoganOpen || !isMGLoggerOk) return
        try {
            val code =
                LoggerWrite(flag, log, localTime, threadName, threadId, if (isMain) 1 else 0)
            if (code != MGLoggerStatus.MGLOGGER_WRITE_SUCCESS || Logger.sDebug) {
                loggerStatusCode(MGLoggerStatus.MGLOGGER_WRITE_STATUS, code)
            }
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
            loggerStatusCode(
                MGLoggerStatus.MGLOGGER_WRITE_STATUS,
                MGLoggerStatus.MGLOGGER_WRITE_FAIL_JNI
            )
        }
    }

    private fun loggerStatusCode(cmd: String, code: Int) {
        if (code < 0) {
            if (cmd.endsWith(MGLoggerStatus.MGLOGGER_WRITE_STATUS) &&
                code != MGLoggerStatus.MGLOGGER_INIT_FAIL_JNI &&
                !errorSet.add(code)
            ) {
                return
            }
            loggerStatus?.loggerStatus(cmd, code)
        }
    }

    @Keep
    @JvmStatic
    public fun onLoggerStatus(code: Int, cmd: String) {
        loggerStatusCode(cmd, code)
    }

    @Keep
    @JvmStatic
    public fun onLogcatCollectorFail() {
        loggerStatusCode(
            MGLoggerStatus.MGLOGGER_LOGCAT_COLLECTOR_STATUS,
            MGLoggerStatus.MGLOGGER_LOGCAT_COLLECTOR_FAIL
        )
    }
}

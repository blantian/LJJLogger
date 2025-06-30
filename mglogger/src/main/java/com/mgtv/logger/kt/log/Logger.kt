package com.mgtv.logger.kt.log

import android.os.Looper
import com.mgtv.logger.kt.i.ILoggerStatus
import com.mgtv.logger.kt.i.ISendLogCallback
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.runBlocking
import kotlin.coroutines.CoroutineContext

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:36
 */
public object Logger : CoroutineScope {
    private val job = SupervisorJob()
    public override val coroutineContext: CoroutineContext = Dispatchers.IO + job

    private lateinit var cfg: LoggerConfig
    private var worker: LoggerActor? = null

    public val isReady: Boolean get() = worker != null

    public fun init(config: LoggerConfig) {
        cfg = config
        worker = LoggerActor(cfg, this)
    }

    public fun init(block: LoggerConfig.Builder.() -> Unit) {
        init(LoggerConfig.Builder().apply(block).build())
    }

    public fun w(log: String, type: Int) {
        ensureReady()
        val threadName = Thread.currentThread().name
        val threadId = Thread.currentThread().id
        val isMainThread = Looper.getMainLooper() == Looper.myLooper()
        worker!!.offer(LogTask.Write(log, type, threadName, threadId, isMainThread))
    }

    public fun getSystemLogs(maxLines: Int = 200): String {
        ensureReady()
        val result = CompletableDeferred<String>()
        worker!!.offer(LogTask.GetSysLog(maxLines, result))
        return runBlocking { result.await() }
    }

    public fun flush() {
        ensureReady()
        worker!!.offer(LogTask.Flush)
    }

    public fun send(
        dates: List<String>,
        strategy: SendLogStrategy,
        callback: ISendLogCallback? = null
    ) {
        ensureReady()
        dates.forEach { worker!!.offer(LogTask.Send(it, strategy, callback)) }
    }

    public fun getAllFilesInfo(): Map<String, Long> =
        worker?.collectFileInfo() ?: emptyMap()

    public fun setDebug(enable: Boolean) { sDebug = enable }

    public fun setStatusListener(listener: ILoggerStatus?): Unit {
        statusListener = listener
    }

    public fun close() {
        if (isReady) {
            worker?.close()
            worker = null
        }
        job.cancel()
    }


    private fun ensureReady() {
        check(isReady) { "MGLogger is not initialized or has been closed" }
    }


    internal var sDebug = false
    internal var statusListener: ILoggerStatus? = null
    internal fun onListenerLogWriteStatus(cmd: String, code: Int) {
        statusListener?.loggerStatus(cmd, code)
    }
}
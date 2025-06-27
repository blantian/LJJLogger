package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.i.ILoggerStatus
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlin.coroutines.CoroutineContext

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:36
 */
object Logger : CoroutineScope {
    private val job = SupervisorJob()
    override val coroutineContext: CoroutineContext = Dispatchers.IO + job

    private lateinit var cfg: LoggerConfig
    private var worker: LoggerActor? = null

    val isReady: Boolean get() = worker != null

    fun init(config: LoggerConfig) {
        cfg = config
        worker = LoggerActor(cfg, this)
    }

    fun init(block: LoggerConfig.Builder.() -> Unit) {
        init(LoggerConfig.Builder().apply(block).build())
    }

    fun w(log: String, type: Int) {
        ensureReady()
        worker!!.offer(LogTask.Write(log, type))
    }

    fun flush() {
        ensureReady()
        worker!!.offer(LogTask.Flush)
    }

    fun send(
        dates: List<String>,
        strategy: SendLogStrategy,
        callback: ((Int, ByteArray?) -> Unit)? = null
    ) {
        ensureReady()
        dates.forEach { worker!!.offer(LogTask.Send(it, strategy, callback)) }
    }

    fun getAllFilesInfo(): Map<String, Long> =
        worker?.collectFileInfo() ?: emptyMap()

    fun setDebug(enable: Boolean) { sDebug = enable }

    fun setStatusListener(listener: ILoggerStatus): Unit {
        statusListener = listener
    }

    fun close() {
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
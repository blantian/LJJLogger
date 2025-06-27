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
    private lateinit var worker: LoggerActor

    val isReady: Boolean get() = this::worker.isInitialized

    fun init(block: LoggerConfig.Builder.() -> Unit) {
        cfg = LoggerConfig.Builder().apply(block).build()
        worker = LoggerActor(cfg, this)
    }

    fun w(log: String, type: Int) {
        ensureReady()
        worker.offer(LogTask.Write(log, type))
    }

    fun flush() {
        ensureReady()
        worker.offer(LogTask.Flush)
    }

    fun send(
        dates: List<String>,
        strategy: SendLogStrategy,
        callback: ((Int, ByteArray?) -> Unit)? = null
    ) {
        ensureReady()
        dates.forEach { worker.offer(LogTask.Send(it, strategy, callback)) }
    }

    fun getAllFilesInfo(): Map<String, Long> =
        if (isReady) worker.collectFileInfo() else emptyMap()

    fun setDebug(enable: Boolean) { sDebug = enable }

    fun close() {
        if (isReady) {
            worker.close()
        }
        job.cancel()
    }

    private fun ensureReady() {
        check(isReady) { "Please initialize MGLogger first" }
    }


    internal var sDebug = false
    internal var statusListener: ILoggerStatus? = null
    internal fun onListenerLogWriteStatus(cmd: String, code: Int) {
        statusListener?.loggerStatus(cmd, code)
    }
}
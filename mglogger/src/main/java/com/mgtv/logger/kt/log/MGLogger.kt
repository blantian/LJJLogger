package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.di.createLoggerModule
import com.mgtv.logger.kt.i.ILoggerStatus
import org.koin.core.context.startKoin
import com.mgtv.logger.kt.i.ISendLogCallback
import com.mgtv.logger.kt.log.Logger
import com.mgtv.logger.kt.log.SendLogStrategy

/**
 * Kotlin facade similar to [Logan] for easy access to [Logger].
 * Created by lantian
 * Date： 2025/6/27
 * Time： 22:38
 */
object MGLogger {

    @JvmStatic
    fun init(
        config: LoggerConfig,
        loggerStatus: ILoggerStatus? = null
    ) {
        startKoin {
            modules(
                createLoggerModule(
                    {
                        putCachePath(config.cachePath)
                        putLogDir(config.logDir)
//                        putKeepDays(config.keepDays)
//                        putMaxFile(config.maxFile)
//                        putMinSdCard(config.minSdCard)
//                        putMaxQueue(config.maxQueue)
//                        putKey16(config.key16)
//                        putIv16(config.iv16)
                    },
                    loggerStatus = loggerStatus
                )
            )
        }
    }

    @JvmStatic
    fun w(log: String, type: Int) {
        Logger.w(log, type)
    }

    fun flush() {
        Logger.flush()
    }

    @JvmStatic
    fun send(
        url: String,
        header: Map<String, String>? = null,
        date: String,
        strategy: SendLogStrategy? = null,
        callback: ISendLogCallback? = null
    ) {
        val strategy = strategy ?: DefaultSendLogStrategy(url,header)
        Logger.send(listOf(date), strategy, callback)
    }

    fun getAllFilesInfo(): Map<String, Long> = Logger.getAllFilesInfo()

    fun setDebug(enable: Boolean) {
        Logger.setDebug(enable)
    }

    fun setStatusListener(listener: ILoggerStatus?) {
        Logger.setStatusListener(listener)
    }

    fun close() {
        Logger.close()
    }

}

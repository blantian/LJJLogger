package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.di.createLoggerModule
import com.mgtv.logger.kt.i.ILoggerStatus
import org.koin.core.context.startKoin
import com.mgtv.logger.kt.i.ISendLogCallback

/**
 * Kotlin facade similar to [Logan] for easy access to [Logger].
 * Created by lantian
 * Date： 2025/7/3
 * Time： 22:38
 */
public object MGLogger {

    @JvmStatic
    @JvmOverloads
    public fun init(
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
    public fun w(log: String, type: Int) {
        Logger.w(log, type)
    }

    @JvmStatic
    public fun getSystemLogs(mode: Int = 1) = Logger.getSystemLogs(mode)

    @JvmStatic
    public fun hookLogs() { Logger.hookLogs() }

    @JvmStatic
    public fun flush() {
        Logger.flush()
    }


    @JvmStatic
    public fun send(
        url: String,
        header: Map<String, String>? = null,
        date: String,
        strategy: SendLogStrategy? = null,
        callback: ISendLogCallback? = null
    ) {
        val strategy = strategy ?: DefaultSendLogStrategy(url, header)
        Logger.send(listOf(date), strategy, callback)
    }

    @JvmStatic
    public fun getAllFilesInfo(): Map<String, Long> = Logger.getAllFilesInfo()


    public fun setDebug(enable: Boolean) {
        Logger.setDebug(enable)
    }

    @JvmStatic
    public fun setStatusListener(listener: ILoggerStatus?) {
        Logger.setStatusListener(listener)
    }

    @JvmStatic
    public fun close() {
        Logger.close()
    }

}

package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.di.createLoggerModule
import com.mgtv.logger.kt.i.ILoggerStatus
import org.koin.core.context.startKoin

/**
 * Description:
 * Created by lantian
 * Date： 2025/6/27
 * Time： 22:38
 */
object MGLogger {

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
                    },
                    loggerStatus = loggerStatus
                )
            )
        }
    }

}
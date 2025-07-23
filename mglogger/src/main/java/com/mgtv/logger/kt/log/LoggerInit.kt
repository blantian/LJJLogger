package com.mgtv.logger.kt.log

import com.mgtv.logger.kt.i.ILoggerStatus

/**
 * Provides Koin module for logger.
 */
public fun createLoggerModule(
    block: LoggerConfig.Builder.() -> Unit,
    loggerStatus: ILoggerStatus? = null
) {
    val config = LoggerConfig.Builder().apply(block).build()
    Logger.apply {
        init(config)
        setStatusListener(loggerStatus)
    }
}

package com.mgtv.logger.kt.di

import com.mgtv.logger.kt.i.ILoggerStatus
import com.mgtv.logger.kt.log.Logger
import com.mgtv.logger.kt.log.LoggerConfig
import org.koin.core.module.Module
import org.koin.dsl.module

/**
 * Provides Koin module for MGLogger.
 */
public fun createLoggerModule(
    block: LoggerConfig.Builder.() -> Unit,
    loggerStatus: ILoggerStatus? = null
): Module {
    val config = LoggerConfig.Builder().apply(block).build()
    return module {
        single { config }
        single(createdAtStart = true) {
            Logger.apply {
                init(config)
                setStatusListener(loggerStatus)
            }
        }
    }
}

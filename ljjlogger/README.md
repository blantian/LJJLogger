# MGLogger

This module provides a coroutine based log system.

## Koin module example

```kotlin
val loggerModule = module {
    single { Logger }
}

Logger.setStatusListener(object : ILoggerStatus {
    override fun loggerStatus(cmd: String, code: Int) {
        // handle status
    }
})
```

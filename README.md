# LJJLogger
LJJLogger是一个收集Android当前运行进程系统日志并加密压缩缓存的高性能日志框架。
两种方式收集日志hook系统日志函数，启用logcat子进程。缓存落盘内置美团CLogan日志框架。
## demo 版本
AGP 8.0.2

jdk 17

## 初始化
现阶段可以直接引入ljjlogger模块到自己项目，后期会支持Maven依赖。初始化流程如下

```java
// 必须使用内部路径，需mmap缓存
File internalDir = new File(getApplicationContext().getFilesDir(), "logcache/");
        if (!internalDir.exists()) {
            boolean isCreated = internalDir.mkdirs();
            if (!isCreated) {
                Log.e(TAG, "Failed to create directory: " + internalDir.getAbsolutePath());
            }
        }
        // 配置黑名单，可过滤对应TAG日志
        ArrayList<String> blackList = new ArrayList<>();
        blackList.add("MyApplication");
        blackList.add("art");
        blackList.add("IPCThreadState");
        blackList.add("dalvikvm");
        // 更详细的配置看 LoggerConfig 配置类
        LoggerConfig loggerConfig = LoggerConfig
                .builder(internalDir.getAbsolutePath(), internalDir.getAbsolutePath() + File.separator + FILE_NAME)
                .nativeLogCacheSelector(0) // 0: hook 系统log, 1: logcat 子进程
                .logcatBlackList(blackList)
                .build();

        LJJLogger.setStatusListener((code, msg) -> {
            Log.i(TAG, "Logger:: code=" + code + " | " + "msg : " + msg);
            if (msg.equals(LJJLoggerStatus.MGLOGGER_INIT_STATUS) && code == LJJLoggerStatus.MGLOGGER_OK) {
                Log.i(TAG, "Logger initialized successfully");
                isInitialized = true;
            } else {
                Log.e(TAG, "Logger initialization failed with code: " + code);
                isInitialized = false;
            }
        });
        LJJLogger.init(loggerConfig);
```

## 日志解密工具

[LoggerUnzip](https://github.com/blantian/LoggerUnzip)


## 好用就给个⭐️ 老铁们。


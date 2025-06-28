package com.mgtv.logger.kt.log

import android.os.Looper
import android.os.StatFs
import com.mgtv.logger.java.Util
import com.mgtv.logger.kt.i.ILoggerProtocol
import com.mgtv.logger.kt.i.ILoggerStatus
import com.mgtv.logger.kt.i.ISendLogCallback
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.flow.receiveAsFlow
import kotlinx.coroutines.launch
import java.io.File
import java.text.ParseException
import java.text.SimpleDateFormat
import java.util.Locale
import java.util.concurrent.atomic.AtomicBoolean

/**
 * Description: Actor 负责顺序消费任务
 * Created by lantian
 * Date： 2025/6/26
 * Time： 14:25
 */

internal class LoggerActor(
    private val cfg: LoggerConfig,
    parentScope: CoroutineScope
) {
    private val channel = Channel<LogTask>(capacity = cfg.maxQueue)
    private val scope = parentScope.coroutineContext
    private val dateFmt = SimpleDateFormat("yyyy-MM-dd", Locale.getDefault())
    private val long24h = 24 * 60 * 60 * 1000L


    private val protocol: ILoggerProtocol = MGLoggerJni.also {
        it.setOnLoggerStatus(object : ILoggerStatus {
            override fun loggerStatus(cmd: String, code: Int) {
                Logger.onListenerLogWriteStatus(cmd, code)
            }
        })
        it.logger_init(cfg.cachePath, cfg.logDir, cfg.maxFile.toInt(), cfg.key16, cfg.iv16)
        it.logger_debug(Logger.sDebug)
    }

    private val isSdWritable = AtomicBoolean(true)
    private var currentDay = 0L
    private var lastSpaceCheck = 0L

    init {
        CoroutineScope(scope).launch {
            channel.receiveAsFlow().collect {
                    task -> handle(task)
            }
        }
    }

    /**
     * 向 Actor 提交日志任务
     * @param task 日志任务
     */
    internal fun offer(task: LogTask): Boolean {
        return channel.trySend(task).isSuccess
    }

    internal fun close() {
        channel.close()
    }

    /**
     * 收集日志文件信息
     * @return 返回日志文件名和大小的映射
     */
    internal fun collectFileInfo(): Map<String, Long> {
        val dir = File(cfg.logDir)
        return dir.listFiles()?.associate {
            Util.getDateStr(it.name.toLong()) to it.length()
        } ?: emptyMap()
    }

    /**
     * 处理日志任务
     * @param task 日志任务
     */
    private suspend fun handle(task: LogTask) {
        when (task) {
            is LogTask.Write -> write(task)
            is LogTask.Flush -> protocol.logger_flush()
            is LogTask.Send -> send(task)
        }
    }

    /**
     * 写入日志任务
     * @param t 日志写入任务
     */
    private fun write(t: LogTask.Write) {

        if (!withinSameDay()) {
            rotateDay()
        }

        // sd‑card 空间检查,每隔一分钟
        val now = System.currentTimeMillis()
        if (now - lastSpaceCheck > 60_000) {
            isSdWritable.set(checkDiskSpace())
            lastSpaceCheck = now
        }
        if (!isSdWritable.get()) return

        protocol.logger_write(
            t.type,
            t.log,
            System.currentTimeMillis(),
            t.threadName,
            t.threadId,
            t.isMainThread
        )
    }

    /**
     * 轮转日志文件到新的一天
     */
    private fun rotateDay() {
        currentDay = Util.getCurrentTime()
        deleteExpired()
        protocol.logger_open(currentDay.toString())
    }

    /**
     * 检查当前时间是否在同一天内
     * @return 如果当前时间在同一天内返回 true，否则返回 false
     */
    private fun withinSameDay(): Boolean {
        val now = System.currentTimeMillis()
        return currentDay in (now - long24h) until now
    }

    /**
     * 删除过期的日志文件
     */
    private fun deleteExpired() {
        val deleteBefore = currentDay - cfg.keepDays
        File(cfg.logDir).listFiles()?.forEach {
            val ts = it.name.split(".").firstOrNull()?.toLongOrNull() ?: return@forEach
            if (ts <= deleteBefore) it.delete()
        }
    }

    /**
     * 发送日志任务
     */
    private suspend fun send(t: LogTask.Send) {
        val dateMillis = try {
            dateFmt.parse(t.date)?.time ?: return
        } catch (e: ParseException) {
            e.printStackTrace()
            return
        }
        val file = File(cfg.logDir, dateMillis.toString())
        if (!file.isFile) return
        // 上传日志文件前检查是否是当天的日志，如果是当天的日志，则同步缓存
        if (dateMillis == currentDay) protocol.logger_flush()

        val (status, body) = t.strategy.send(file)
        t.callback?.onLogSendCompleted(status, body)
    }

    /**
     * 检查 SD 卡空间是否足够
     */
    private fun checkDiskSpace(): Boolean {
        return try {
            val stat = StatFs(cfg.logDir)
            val available = stat.availableBlocks.toLong() * stat.blockSize
            available > cfg.minSdCard
        } catch (e: IllegalArgumentException) {
            false
        }
    }
}
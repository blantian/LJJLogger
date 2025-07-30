package com.mgtv.logger.kt.log

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileInputStream
import java.net.HttpURLConnection
import java.net.URL
import javax.net.ssl.HostnameVerifier
import javax.net.ssl.HttpsURLConnection
import javax.net.ssl.SSLSession

/**
 * Description:
 * Created by lantian
 * Date： 2025/7/3
 * Time： 15:10
 */
public class DefaultSendLogStrategy(
    private val uploadUrl: String,
    private val headers: Map<String, String> ? = emptyMap()
) : SendLogStrategy {
    public override suspend fun send(logFile: File): Pair<Int, ByteArray?> = withContext(Dispatchers.IO) {
        var conn: HttpURLConnection? = null
        var status = -1
        var body: ByteArray? = null
        try {
            conn = (URL(uploadUrl).openConnection() as HttpURLConnection).apply {
                if (this is HttpsURLConnection) {
                    hostnameVerifier = HostnameVerifier { _: String?, _: SSLSession? -> true }
                }
                headers?.forEach { (k, v) -> addRequestProperty(k, v) }
                readTimeout = 15000
                connectTimeout = 15000
                doInput = true
                doOutput = true
                requestMethod = "POST"
            }
            FileInputStream(logFile).use { fis ->
                conn.outputStream.use { os ->
                    fis.copyTo(os)
                    os.flush()
                }
            }
            status = conn.responseCode
            if (status / 100 == 2) {
                val buffer = ByteArray(2048)
                val baos = ByteArrayOutputStream()
                conn.inputStream.use { stream ->
                    var len: Int
                    while (stream.read(buffer).also { len = it } != -1) baos.write(buffer, 0, len)
                }
                body = baos.toByteArray()
            }
        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            conn?.disconnect()
        }
        status to body
    }
}
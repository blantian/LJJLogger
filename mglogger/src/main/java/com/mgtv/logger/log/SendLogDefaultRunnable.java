package com.mgtv.logger.log;

import android.util.Log;

import androidx.annotation.NonNull;

import com.mgtv.logger.log.i.SendLogCallback;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;

public class SendLogDefaultRunnable extends SendLogRunnable{

    private static final String TAG = "SendLogDefaultRunnable";

    private volatile String uploadUrl;
    private final Map<String, String> requestHeaders = Collections.synchronizedMap(new HashMap<>());
    private volatile SendLogCallback callback;

    /** 设置上传地址 */
    public void setUrl(@NonNull String url) {
        this.uploadUrl = url;
    }

    /** 设置额外请求头 */
    public void setRequestHeader(Map<String, String> headers) {
        requestHeaders.clear();
        if (headers != null) requestHeaders.putAll(headers);
    }

    /** 上传完成回调 */
    public void setSendLogCallback(SendLogCallback cb) { this.callback = cb; }

    @Override
    protected void sendLog(@NonNull File logFile) throws Exception {
        if (uploadUrl == null) throw new IllegalStateException("uploadUrl not set");
        int code = postFile(logFile, uploadUrl, requestHeaders);
        if (callback != null) callback.onLogSendCompleted(code, null);
        Log.d(TAG, "log send completed, http statusCode : " + code);
    }

    private int postFile(File file, String urlStr, Map<String, String> headers) throws Exception {
        HttpURLConnection conn = null;
        try {
            URL url = new URL(urlStr);
            conn = (HttpURLConnection) url.openConnection();
            if (conn instanceof HttpsURLConnection) {
                ((HttpsURLConnection) conn).setHostnameVerifier(ALLOW_ALL_HOSTNAME_VERIFIER);
            }
            conn.setReadTimeout(15000);
            conn.setConnectTimeout(15000);
            conn.setRequestMethod("POST");
            conn.setDoOutput(true);
            conn.setDoInput(true);
            conn.setFixedLengthStreamingMode(file.length());

            for (Map.Entry<String, String> e : headers.entrySet()) {
                conn.addRequestProperty(e.getKey(), e.getValue());
            }

            try (BufferedInputStream in = new BufferedInputStream(new FileInputStream(file));
                 BufferedOutputStream out = new BufferedOutputStream(conn.getOutputStream())) {
                byte[] buf = new byte[8 * 1024];
                int len;
                while ((len = in.read(buf)) != -1) {
                    out.write(buf, 0, len);
                }
                out.flush();
            }

            return conn.getResponseCode();
        } finally {
            if (conn != null) conn.disconnect();
        }
    }

    private static final HostnameVerifier ALLOW_ALL_HOSTNAME_VERIFIER = (hostname, session) -> true;
}

package com.lt.ljjlogger;

public class SendAction {
    String uploadPath; // 上传路径
    SendLogRunnable sendLogRunnable; // 上传任务
    boolean isValid() {
        return sendLogRunnable != null && !uploadPath.isEmpty();
    }
}

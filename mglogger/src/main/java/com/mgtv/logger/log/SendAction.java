package com.mgtv.logger.log;

class SendAction {

    long fileSize; //文件大小

    String date; //日期

    String uploadPath;

    SendLogRunnable sendLogRunnable;

    boolean isValid() {
        boolean valid = false;
        if (sendLogRunnable != null) {
            valid = true;
        } else if (fileSize > 0) {
            valid = true;
        }
        return valid;
    }
}

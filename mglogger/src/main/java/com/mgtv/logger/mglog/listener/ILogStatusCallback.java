package com.mgtv.logger.mglog.listener;

/**
 * 创建时间：2024/2/22 14:38
 * 作 者：向庚平
 * 邮 箱：gegnping@mgtv.com
 * 文件描述：
 */
public interface ILogStatusCallback {
    /**
     * 开始日志收集
     */
    void onLogCollectStart();

    /**
     * 日志收集完成的回调
     */
    void onLogCollectComplete();
}

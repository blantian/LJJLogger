package com.mgtv.logger.mglog;

import android.content.Context;

import java.io.File;
import java.io.OutputStream;
import java.util.List;

/**
 * Created by wukeqi on 2023/11/30.
 * keqi@mgtv.com
 */
public abstract class ILogProxy {

   /**
    * 获取上下文
    * @return
    */
   public abstract Context getAppContext();

   /**
    * 获取当前应用包名
    * @return
    */
   public abstract String getPackageName();

   /**
    * 销毁log相关进程
    */
   public abstract void killLogcatProcess();

   /**
    * 初始化日志打印监听
    *
    * @param logListener
    */
   public abstract void initMGLogListener(ILogListener logListener);


   /**
    * debug 级别日志打印
    * @param tag
    * @param info
    */
   public abstract void d(String tag,String info);

   /**
    * info级别日志打印
    * @param tag
    * @param info
    */
   public abstract void i(String tag,String info);

   /**
    * debug日志开关是否打开
    * @return
    */
   public abstract boolean isDebugOn();

   /**
    * 字符串是否为空
    * @param str
    * @return
    */
   public abstract boolean isEqualsNull(String str);

   /**
    * 获取当前时间点
    * @param format
    * @return
    */
   public abstract String getCurrentTime(String format);

   /**
    * 获取sd卡缓存地址
    * @return
    */
   public abstract File getExternalCacheDir();

   /**
    * 获取内部储存缓存地址
    * @return
    */
   public abstract File getInternalCacheDir();

   /**
    * 获取文件
    * @param path
    * @return
    */
   public abstract File getFileByAbsolutePath(String path);

   /**
    * 将文件夹中文件根据日期排序文件
    * @param fileDir
    * @return
    */
   public abstract File[] orderByDate(String fileDir);

   /**
    * 关闭IO流
    * @param out
    */
   public abstract void closeIOStream(OutputStream out);

   /**
    * 对文件列表压缩加密
    */
   public abstract void doZipFilesWithPassword(File zipFile, char[] password, List<File> srcFiles,
                                             String appendName, File anotherFile);

   /**
    * 在线程中执行
    * @param runnable
    */
   public abstract void runInThread(Runnable runnable);

}

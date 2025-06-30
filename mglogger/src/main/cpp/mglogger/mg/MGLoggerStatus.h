/**
 * Description:
 * Created by lantian 
 * Date： 2025/6/30
 * Time： 10:52
 */


#ifndef MGLOGGER_MGLOGGERSTATUS_H
#define MGLOGGER_MGLOGGERSTATUS_H

#define CLGOAN_INIT_STATUS "mglogger_init" //初始化函数
#define MGLOGGER_INIT_SUCCESS_MMAP 2001 //初始化成功, mmap内存
#define MGLOGGER_INIT_SUCCESS_MEMORY 2002 //初始化成功, 堆内存
#define MGLOGGER_INIT_FAIL_NOCACHE 2010 //初始化失败 , 没有缓存
#define MGLOGGER_INIT_FAIL_NOMALLOC  2011 //初始化失败 , 没有堆内存
#define MGLOGGER_INIT_FAIL_HEADER  2012 //初始化失败 , 初始化头失败
#define MGLOGGER_INIT_INITPUBLICKEY_ERROR  2013 //初始化失败，初始化公钥失败
#define MGLOGGER_INIT_INITBLICKENCRYPT_ERROR 2014 //初始化失败，公钥加密失败
#define MGLOGGER_INIT_INITBASE64_ERROR 2015 //初始化失败，base64失败
#define MGLOGGER_INIT_INITMALLOC_ERROR 2016 //初始化失败 malloc分配失败

#define MGLOGGER_OPEN_STATUS "mglogger_open" //打开文件函数
#define MGLOGGER_OPEN_SUCCESS 3001 //打开文件成功
#define MGLOGGER_OPEN_FAIL_IO 3002 //打开文件IO失败
#define MGLOGGER_OPEN_FAIL_ZLIB 3010 //打开文件zlib失败
#define MGLOGGER_OPEN_FAIL_MALLOC 3011 //打开文件malloc失败
#define MGLOGGER_OPEN_FAIL_NOINIT 3012 //打开文件没有初始化失败
#define MGLOGGER_OPEN_FAIL_HEADER 3013 //打开文件头失败

#define MGLOGGER_WRITE_STATUS "mglogger_write" //写入函数
#define MGLOGGER_WRITE_SUCCESS 4001 //写入日志成功
#define MGLOGGER_WRITE_FAIL_PARAM 4010 //写入失败, 可变参数错误
#define MGLOGGER_WRITE_FAIL_MAXFILE 4011 //写入失败,超过文件最大值
#define MGLOGGER_WRITE_FAIL_MALLOC 4012 //写入失败,malloc失败
#define MGLOGGER_WRITE_FAIL_HEADER 4013 //写入头失败

#define MGLOGGER_FLUSH_STATUS "mglogger_flush" //强制写入函数
#define MGLOGGER_FLUSH_SUCCESS 5001 //fush日志成功
#define MGLOGGER_FLUSH_FAIL_INIT 5010 //初始化失败,日志flush不成功

#endif //MGLOGGER_MGLOGGERSTATUS_H

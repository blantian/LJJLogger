

#ifndef CLOGAN_LOGAN_CONFIG_H
#define CLOGAN_LOGAN_CONFIG_H

#include <zlib.h>
#include <stdio.h>

#define LOGAN_VERSION_KEY "logan_version"
#define LOGAN_PATH_KEY "file"

#define  LOGAN_WRITE_PROTOCOL_HEADER '\1'
#define  LOGAN_WRITE_PROTOCOL_TAIL '\0'

#define LOGAN_CACHE_DIR "logan_cache"
#define LOGAN_CACHE_FILE "logan.mmap2"

#define LOGAN_MMAP_HEADER_PROTOCOL '\15' //MMAP的头文件标识符
#define LOGAN_MMAP_TAIL_PROTOCOL '\16' //MMAP尾文件标识符
#define LOGAN_MMAP_TOTALLEN  3 // MMAP文件长度

#define LOGAN_MAX_GZIP_UTIL 5 * 1024 // gzip的最大压缩单元,5k

#define LOGAN_WRITEPROTOCOL_HEAER_LENGTH 5 //Logan写入协议的头和写入数据的总长度

#define LOGAN_WRITEPROTOCOL_DEVIDE_VALUE 3 //多少分之一写入

#define LOGAN_DIVIDE_SYMBOL "/"

#define LOGAN_LOGFILE_MAXLENGTH 10 * 1024 * 1024 // 日志文件最大长度,10M

#define LOGAN_WRITE_SECTION 20 * 1024 // 写入分片的大小,20k

#define LOGAN_RETURN_SYMBOL "\n"

#define LOGAN_FILE_NONE 0
#define LOGAN_FILE_OPEN 1
#define LOGAN_FILE_CLOSE 2

#define CLOGAN_EMPTY_FILE 0

#define CLOGAN_VERSION_NUMBER 3 //Logan的版本号(2)版本

typedef struct logan_model_struct {
    int total_len; //数据长度
    char *file_path; //文件路径

    int is_malloc_zlib;
    z_stream *strm;
    int zlib_type; //压缩类型
    char remain_data[16]; //剩余空间
    int remain_data_len; //剩余空间长度

    int is_ready_gzip; //是否可以gzip

    int file_stream_type; //文件流类型
    FILE *file; //文件流

    long file_len; //文件大小

    unsigned char *buffer_point; //缓存的指针 (不变)
    unsigned char *last_point; //最后写入位置的指针
    unsigned char *total_point; //总数的指针 (可能变) , 给c看,低字节
    unsigned char *content_lent_point;//协议内容长度指针 , 给java看,高字节
    int content_len; //内容的大小

    unsigned char aes_iv[16]; //aes_iv
    int is_ok;

} cLogan_model;

#endif //CLOGAN_LOGAN_CONFIG_H

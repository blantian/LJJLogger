
#ifndef CLOGAN_ZLIB_UTIL_H
#define CLOGAN_ZLIB_UTIL_H

#include "logan_config.h"
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#define LOGAN_CHUNK 16384

//定义Logan_zlib的状态类型

#define LOGAN_ZLIB_NONE 0
#define LOGAN_ZLIB_INIT 1
#define LOGAN_ZLIB_ING  2
#define LOGAN_ZLIB_END  3
#define LOGAN_ZLIB_FAIL 4

int init_zlib_clogan(cLogan_model *model); //初始化Logan

void clogan_zlib_compress(cLogan_model *model, char *data, int data_len); //压缩文件

void clogan_zlib_end_compress(cLogan_model *model); //压缩结束

void clogan_zlib_delete_stream(cLogan_model *model); //删除初始化的z_stream

#endif //CLOGAN_ZLIB_UTIL_H

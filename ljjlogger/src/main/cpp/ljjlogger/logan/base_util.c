

#include <sys/time.h>
#include <memory.h>
#include "console_util.h"

#define LOGAN_BYTEORDER_NONE  0
#define LOGAN_BYTEORDER_HIGH 1
#define LOGAN_BYTEORDER_LOW 2

//获取毫秒时间戳
long long get_system_current_clogan(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long time = ((long long) tv.tv_sec) * 1000 + ((long long) tv.tv_usec) / 1000;
    return time;
}

//是否为空字符串
int is_string_empty_clogan(char *item) {
    int flag = 1;
    if (NULL != item && strnlen(item, 10) > 0) {
        flag = 0;
    }
    return flag;
}

// 检查CPU的字节序
int cpu_byteorder_clogan(void) {
    static int LOGAN_BYTEORDER = LOGAN_BYTEORDER_NONE;
    if (LOGAN_BYTEORDER == LOGAN_BYTEORDER_NONE) {
        union {
            int i;
            char c;
        } t;
        t.i = 1;
        if (t.c == 1) {
            LOGAN_BYTEORDER = LOGAN_BYTEORDER_LOW;
            printf_clogan("cpu_byteorder_clogan > system is a low byteorder\n");
        } else {
            LOGAN_BYTEORDER = LOGAN_BYTEORDER_HIGH;
            printf_clogan("cpu_byteorder_clogan > system is a high byteorder\n");
        }
    }
    return LOGAN_BYTEORDER;
}

/**
 * 调整字节序,默认传入的字节序为低字节序,如果系统为高字节序,转化为高字节序
 * c语言字节序的写入是低字节序的,读取默认也是低字节序的
 * java语言字节序默认是高字节序的
 * @param data data
 */
void adjust_byteorder_clogan(char data[4]) {
    if (cpu_byteorder_clogan() == LOGAN_BYTEORDER_HIGH) {
        char *temp = data;
        char data_temp = *temp;
        *temp = *(temp + 3);
        *(temp + 3) = data_temp;
        data_temp = *(temp + 1);
        *(temp + 1) = *(temp + 2);
        *(temp + 2) = data_temp;
    }
}

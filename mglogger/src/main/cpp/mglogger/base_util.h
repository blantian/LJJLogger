

#ifndef CLOGAN_BASE_UTIL_H
#define CLOGAN_BASE_UTIL_H

//获取时间毫秒
long long get_system_current_clogan(void);

//判断String是否为空
int is_string_empty_clogan(char *item);

/**
 * 检查CPU的字节序
 * @return 1，高字节序 2，低字节序
 */
int cpu_byteorder_clogan(void);

/**
 * 调整字节序,默认传入的字节序为低字节序,如果系统为高字节序,转化为高字节序
 * c语言字节序的写入是低字节序的,读取默认也是低字节序的
 * java语言字节序默认是高字节序的
 * @param data
 */
void adjust_byteorder_clogan(char item[4]);

#endif //CLOGAN_BASE_UTIL_H

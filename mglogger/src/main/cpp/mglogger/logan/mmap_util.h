
#ifndef LOGAN_MMAP_MMAP
#define LOGAN_MMAP_MMAP 1
#endif

#ifndef LOGAN_MMAP_MEMORY
#define LOGAN_MMAP_MEMORY 0
#endif

#ifndef LOGAN_MMAP_FAIL
#define LOGAN_MMAP_FAIL -1
#endif

#ifndef LOGAN_MMAP_LENGTH
#define LOGAN_MMAP_LENGTH 50 * 1024 //50k
#endif

#ifndef LOGAN_MEMORY_LENGTH
#define LOGAN_MEMORY_LENGTH 50 * 1024 //50k
#endif

#ifndef CLOGAN_MMAP_UTIL_H
#define CLOGAN_MMAP_UTIL_H

#include <stdio.h>
#include <unistd.h>
#include<sys/mman.h>
#include <fcntl.h>
#include <string.h>

int open_mmap_file_clogan(char *_filepath, unsigned char **buffer, unsigned char **cache);

#endif //CLOGAN_MMAP_UTIL_H

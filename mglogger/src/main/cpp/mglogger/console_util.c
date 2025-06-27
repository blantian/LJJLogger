

#include <stdarg.h>
#include <stdio.h>
#include "console_util.h"

static int is_debug_logan = 0;

int printf_clogan(char *fmt, ...) {
    int cnt = 0;
    if (is_debug_logan) {
        va_list argptr;
        va_start(argptr, fmt);
        cnt = vprintf(fmt, argptr);
        va_end(argptr);
    }
    return (cnt);
}

void set_debug_clogan(int debug) {
    is_debug_logan = debug;
}

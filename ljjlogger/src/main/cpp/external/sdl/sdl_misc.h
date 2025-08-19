

#ifndef AV_MGLOGGER_SDL__MISC_H
#define AV_MGLOGGER_SDL__MISC_H

#include <stdlib.h>
#include <memory.h>

#ifndef SDL_MAX
#define SDL_MAX(a, b)    ((a) > (b) ? (a) : (b))
#endif

#ifndef SDL_MIN
#define SDL_MIN(a, b)    ((a) < (b) ? (a) : (b))
#endif

#ifndef SDL_ALIGN
#define SDL_ALIGN(x, align) ((( x ) + (align) - 1) / (align) * (align))
#endif

#define AV_MGLOGGER_CHECK_RET(condition__, retval__, ...) \
    if (!(condition__)) { \
        ALOGE(__VA_ARGS__); \
        return (retval__); \
    }

#ifndef NELEM
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

inline static void *mallocz(size_t size)
{
    void *mem = malloc(size);
    if (!mem)
        return mem;

    memset(mem, 0, size);
    return mem;
}

inline static void freep(void **mem)
{
    if (mem && *mem) {
        free(*mem);
        *mem = NULL;
    }
}

#endif

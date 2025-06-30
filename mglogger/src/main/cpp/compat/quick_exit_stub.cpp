#include <android/api-level.h>
#include <stdlib.h>
#if __ANDROID_API__ < 21
extern "C" int at_quick_exit(void (*func)(void)) {
    return atexit(func);
}

extern "C" void quick_exit(int status) {
    exit(status);
}
#endif

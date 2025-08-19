

#include "j4a_allclasses.h"

int J4A_LoadAll__catchAll(JNIEnv *env)
{
    int ret = 0;

    // load android.os.Build at very beginning
    J4A_LOAD_CLASS(android_os_Build);

#include "j4a_allclasses.loader.h"

fail:
    return ret;
}

#include "j4a_base.h"
#include "external/j4a/Android/os/Build.h"

/********************
 * Exception Handle
 ********************/

bool J4A_ExceptionCheck__throwAny(JNIEnv *env)
{
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        return true;
    }

    return false;
}

bool J4A_ExceptionCheck__catchAll(JNIEnv *env)
{
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return true;
    }

    return false;
}

int J4A_ThrowExceptionOfClass(JNIEnv* env, jclass clazz, const char* msg)
{
    if ((*env)->ThrowNew(env, clazz, msg) != JNI_OK)
        J4A_ALOGE("%s: Failed: msg: '%s'\n", __func__, msg);

    return 0;
}

int J4A_ThrowException(JNIEnv* env, const char* class_sign, const char* msg)
{
    int ret = -1;

    if (J4A_ExceptionCheck__catchAll(env)) {
        J4A_ALOGE("pending exception throwed.\n");
    }

    jclass exceptionClass = J4A_FindClass__catchAll(env, class_sign);
    if (exceptionClass == NULL) {
        J4A_FUNC_FAIL_TRACE();
        ret = -1;
        goto fail;
    }

    ret = J4A_ThrowExceptionOfClass(env, exceptionClass, msg);
    if (ret) {
        J4A_FUNC_FAIL_TRACE();
        goto fail;
    }

    ret = 0;
fail:
    J4A_DeleteLocalRef__p(env, &exceptionClass);
    return ret;
}

int J4A_ThrowIllegalStateException(JNIEnv *env, const char* msg)
{
    return J4A_ThrowException(env, "java/lang/IllegalStateException", msg);
}

/********************
 * References
 ********************/

jclass J4A_NewGlobalRef__catchAll(JNIEnv *env, jobject obj)
{
    jclass obj_global = (*env)->NewGlobalRef(env, obj);
    if (J4A_ExceptionCheck__catchAll(env) || !(obj_global)) {
        J4A_FUNC_FAIL_TRACE();
        goto fail;
    }

fail:
    return obj_global;
}

void J4A_DeleteLocalRef(JNIEnv *env, jobject obj)
{
    if (!obj)
        return;
    (*env)->DeleteLocalRef(env, obj);
}

void J4A_DeleteLocalRef__p(JNIEnv *env, jobject *obj)
{
    if (!obj)
        return;
    J4A_DeleteLocalRef(env, *obj);
    *obj = NULL;
}

void J4A_DeleteGlobalRef(JNIEnv *env, jobject obj)
{
    if (!obj)
        return;
    (*env)->DeleteGlobalRef(env, obj);
}

void J4A_DeleteGlobalRef__p(JNIEnv *env, jobject *obj)
{
    if (!obj)
        return;
    J4A_DeleteGlobalRef(env, *obj);
    *obj = NULL;
}

void J4A_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *c_str)
{
    if (!str || !c_str)
        return;
    (*env)->ReleaseStringUTFChars(env, str, c_str);
}

void J4A_ReleaseStringUTFChars__p(JNIEnv *env, jstring str, const char **c_str)
{
    if (!str || !c_str)
        return;
    J4A_ReleaseStringUTFChars(env, str, *c_str);
    *c_str = NULL;
}

/********************
 * Class Load
 ********************/

jclass J4A_FindClass__catchAll(JNIEnv *env, const char *class_sign)
{
    jclass clazz = (*env)->FindClass(env, class_sign);
    if (J4A_ExceptionCheck__catchAll(env) || !(clazz)) {
        J4A_FUNC_FAIL_TRACE();
        clazz = NULL;
        goto fail;
    }

fail:
    return clazz;
}

jclass J4A_FindClass__asGlobalRef__catchAll(JNIEnv *env, const char *class_sign)
{
    jclass clazz_global = NULL;
    jclass clazz = J4A_FindClass__catchAll(env, class_sign);
    if (!clazz) {
        J4A_FUNC_FAIL_TRACE1(class_sign);
        goto fail;
    }

    clazz_global = J4A_NewGlobalRef__catchAll(env, clazz);
    if (!clazz_global) {
        J4A_FUNC_FAIL_TRACE1(class_sign);
        goto fail;
    }

fail:
    J4A_DeleteLocalRef__p(env, &clazz);
    return clazz_global;
}

jmethodID J4A_GetMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign)
{
    jmethodID method_id = (*env)->GetMethodID(env, clazz, method_name, method_sign);
    if (J4A_ExceptionCheck__catchAll(env) || !method_id) {
        J4A_FUNC_FAIL_TRACE2(method_name, method_sign);
        method_id = NULL;
        goto fail;
    }

fail:
    return method_id;
}

jmethodID J4A_GetStaticMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign)
{
    jmethodID method_id = (*env)->GetStaticMethodID(env, clazz, method_name, method_sign);
    if (J4A_ExceptionCheck__catchAll(env) || !method_id) {
        J4A_FUNC_FAIL_TRACE2(method_name, method_sign);
        method_id = NULL;
        goto fail;
    }

fail:
    return method_id;
}

jfieldID J4A_GetFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *field_sign)
{
    jfieldID field_id = (*env)->GetFieldID(env, clazz, field_name, field_sign);
    if (J4A_ExceptionCheck__catchAll(env) || !field_id) {
        J4A_FUNC_FAIL_TRACE2(field_name, field_sign);
        field_id = NULL;
        goto fail;
    }

fail:
    return field_id;
}

jfieldID J4A_GetStaticFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *field_sign)
{
    jfieldID field_id = (*env)->GetStaticFieldID(env, clazz, field_name, field_sign);
    if (J4A_ExceptionCheck__catchAll(env) || !field_id) {
        J4A_FUNC_FAIL_TRACE2(field_name, field_sign);
        field_id = NULL;
        goto fail;
    }

fail:
    return field_id;
}

/********************
 * Misc Functions
 ********************/

jbyteArray J4A_NewByteArray__catchAll(JNIEnv *env, jsize capacity)
{
    jbyteArray local = (*env)->NewByteArray(env, capacity);
    if (J4A_ExceptionCheck__catchAll(env) || !local)
        return NULL;

    return local;
}

jbyteArray J4A_NewByteArray__asGlobalRef__catchAll(JNIEnv *env, jsize capacity)
{
    jbyteArray local = (*env)->NewByteArray(env, capacity);
    if (J4A_ExceptionCheck__catchAll(env) || !local)
        return NULL;

    jbyteArray global = (*env)->NewGlobalRef(env, local);
    J4A_DeleteLocalRef__p(env, &local);
    return global;
}

int J4A_GetSystemAndroidApiLevel(JNIEnv *env)
{
    static int SDK_INT = 0;
    if (SDK_INT > 0)
        return SDK_INT;

    SDK_INT = J4AC_android_os_Build__VERSION__SDK_INT__get__catchAll(env);
    J4A_ALOGI("API-Level: %d\n", SDK_INT);
    return SDK_INT;
}

/********************
 * JNI Environment
 ********************/
// 添加全局JavaVM指针
static JavaVM *g_j4a_jvm;

// 提供设置JavaVM的函数，在JNI_OnLoad中调用
void J4A_SetJavaVM(JavaVM *vm)
{
    g_j4a_jvm = vm;
}


jobject J4A_GetFileDescriptor__catchAll(JNIEnv *env, int fd)
{
    static jclass FileDescriptor_class = NULL;
    static jmethodID FileDescriptor_constructor = NULL;
    static jfieldID FileDescriptor_descriptor = NULL;
    
    jobject fileDescriptor = NULL;
    
    if (FileDescriptor_class == NULL) {
        FileDescriptor_class = (*env)->FindClass(env, "java/io/FileDescriptor");
        if (FileDescriptor_class == NULL || (*env)->ExceptionCheck(env)) {
            J4A_ALOGE("Failed to find java.io.FileDescriptor class");
            (*env)->ExceptionClear(env);
            return NULL;
        }
        
        FileDescriptor_class = (jclass)(*env)->NewGlobalRef(env, FileDescriptor_class);
        if (FileDescriptor_class == NULL) {
            J4A_ALOGE("Failed to create global reference for FileDescriptor class");
            return NULL;
        }
        
        FileDescriptor_constructor = (*env)->GetMethodID(env, FileDescriptor_class, "<init>", "()V");
        if (FileDescriptor_constructor == NULL || (*env)->ExceptionCheck(env)) {
            J4A_ALOGE("Failed to get FileDescriptor constructor");
            (*env)->ExceptionClear(env);
            return NULL;
        }
        
        FileDescriptor_descriptor = (*env)->GetFieldID(env, FileDescriptor_class, "descriptor", "I");
        if (FileDescriptor_descriptor == NULL || (*env)->ExceptionCheck(env)) {
            J4A_ALOGE("Failed to get FileDescriptor.descriptor field");
            (*env)->ExceptionClear(env);
            return NULL;
        }
    }
    
    fileDescriptor = (*env)->NewObject(env, FileDescriptor_class, FileDescriptor_constructor);
    if (fileDescriptor == NULL || (*env)->ExceptionCheck(env)) {
        J4A_ALOGE("Failed to create new FileDescriptor object");
        (*env)->ExceptionClear(env);
        return NULL;
    }
    
    (*env)->SetIntField(env, fileDescriptor, FileDescriptor_descriptor, fd);
    if ((*env)->ExceptionCheck(env)) {
        J4A_ALOGE("Failed to set descriptor field");
        (*env)->ExceptionClear(env);
        (*env)->DeleteLocalRef(env, fileDescriptor);
        return NULL;
    }
    
    return fileDescriptor;
}

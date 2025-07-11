# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.8.0)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("mglogger")

set(CMAKE_BUILD_TYPE DEBUG)


# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MbedTLS options
option(USE_STATIC_MBEDTLS_LIBRARY "Build mbed TLS static library." ON)
option(USE_SHARED_MBEDTLS_LIBRARY "Build mbed TLS shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link mbed TLS library to pthread." OFF)

include_directories(./)
include_directories(mglogger)
include_directories(mglogger/mg)
include_directories(mglogger/logan)
include_directories(external/include)
include_directories(external/include/mbedtls)

# Find Android log library
find_library(
    log-lib
    log
)

# Main source files (removed duplicates)
set(SOURCE_FILES
        mglogger/logan/clogan_core.c
        mglogger/logan/mmap_util.c
        mglogger/logan/cJSON.c
        mglogger/logan/construct_data.c
        mglogger/logan/zlib_util.c
        mglogger/logan/json_util.c
        mglogger/logan/aes_util.c
        mglogger/logan/directory_util.c
        mglogger/logan/base_util.c
        mglogger/logan/console_util.c
        mglogger/logan/main.c
        mglogger/mg/Logreader.cpp
        mglogger/mg/HookLoglib.cpp
        mglogger/mg/LoggerHook.cpp
        mglogger/mg/CLoganCaller.cpp
)

# MbedTLS crypto source files
set(src_crypto
        external/library/aes.c
        external/library/aesni.c
        external/library/arc4.c
        external/library/asn1parse.c
        external/library/asn1write.c
        external/library/base64.c
        external/library/bignum.c
        external/library/blowfish.c
        external/library/camellia.c
        external/library/ccm.c
        external/library/cipher.c
        external/library/cipher_wrap.c
        external/library/cmac.c
        external/library/ctr_drbg.c
        external/library/des.c
        external/library/dhm.c
        external/library/ecdh.c
        external/library/ecdsa.c
        external/library/ecjpake.c
        external/library/ecp.c
        external/library/ecp_curves.c
        external/library/entropy.c
        external/library/entropy_poll.c
        external/library/error.c
        external/library/gcm.c
        external/library/havege.c
        external/library/hmac_drbg.c
        external/library/md.c
        external/library/md2.c
        external/library/md4.c
        external/library/md5.c
        external/library/md_wrap.c
        external/library/memory_buffer_alloc.c
        external/library/oid.c
        external/library/padlock.c
        external/library/pem.c
        external/library/pk.c
        external/library/pk_wrap.c
        external/library/pkcs12.c
        external/library/pkcs5.c
        external/library/pkparse.c
        external/library/pkwrite.c
        external/library/platform.c
        external/library/ripemd160.c
        external/library/rsa.c
        external/library/sha1.c
        external/library/sha256.c
        external/library/sha512.c
        external/library/threading.c
        external/library/timing.c
        external/library/version.c
        external/library/version_features.c
        external/library/xtea.c
)

# MbedTLS X509 source files
set(src_x509
        external/library/certs.c
        external/library/pkcs11.c
        external/library/x509.c
        external/library/x509_create.c
        external/library/x509_crl.c
        external/library/x509_crt.c
        external/library/x509_csr.c
        external/library/x509write_crt.c
        external/library/x509write_csr.c
)

# MbedTLS TLS source files
set(src_tls
        external/library/debug.c
        external/library/net_sockets.c
        external/library/ssl_cache.c
        external/library/ssl_ciphersuites.c
        external/library/ssl_cli.c
        external/library/ssl_cookie.c
        external/library/ssl_srv.c
        external/library/ssl_ticket.c
        external/library/ssl_tls.c
)

# Compiler flags
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif (CMAKE_COMPILER_IS_GNUCC)

if (CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wno-documentation-deprecated-sync -Wunreachable-code")
endif (CMAKE_COMPILER_IS_CLANG)

# Platform specific libraries
set(libs "")
if (WIN32)
    set(libs ${libs} ws2_32)
endif (WIN32)

if (USE_PKCS11_HELPER_LIBRARY)
    set(libs ${libs} pkcs11-helper)
endif (USE_PKCS11_HELPER_LIBRARY)

if (ENABLE_ZLIB_SUPPORT)
    set(libs ${libs} ${ZLIB_LIBRARIES})
endif (ENABLE_ZLIB_SUPPORT)

if (LINK_WITH_PTHREAD)
    set(libs ${libs} pthread)
endif ()

# MbedTLS library configuration
if (NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)
    message(FATAL_ERROR "Need to choose static or shared mbedtls build!")
endif (NOT USE_STATIC_MBEDTLS_LIBRARY AND NOT USE_SHARED_MBEDTLS_LIBRARY)

if (USE_STATIC_MBEDTLS_LIBRARY AND USE_SHARED_MBEDTLS_LIBRARY)
    set(mbedtls_static_target "mbedtls_static")
    set(mbedx509_static_target "mbedx509_static")
    set(mbedcrypto_static_target "mbedcrypto_static")
elseif (USE_STATIC_MBEDTLS_LIBRARY)
    set(mbedtls_static_target "mbedtls")
    set(mbedx509_static_target "mbedx509")
    set(mbedcrypto_static_target "mbedcrypto")
endif ()

# Build static MbedTLS libraries
if (USE_STATIC_MBEDTLS_LIBRARY)
    add_library(${mbedcrypto_static_target} STATIC ${src_crypto})
    set_target_properties(${mbedcrypto_static_target} PROPERTIES OUTPUT_NAME mbedcrypto)
    target_link_libraries(${mbedcrypto_static_target} ${libs})

    add_library(${mbedx509_static_target} STATIC ${src_x509})
    set_target_properties(${mbedx509_static_target} PROPERTIES OUTPUT_NAME mbedx509)
    target_link_libraries(${mbedx509_static_target} ${libs} ${mbedcrypto_static_target})

    add_library(${mbedtls_static_target} STATIC ${src_tls})
    set_target_properties(${mbedtls_static_target} PROPERTIES OUTPUT_NAME mbedtls)
    target_link_libraries(${mbedtls_static_target} ${libs} ${mbedx509_static_target})
endif (USE_STATIC_MBEDTLS_LIBRARY)

# Build shared MbedTLS libraries
if (USE_SHARED_MBEDTLS_LIBRARY)
    add_library(mbedcrypto SHARED ${src_crypto})
    set_target_properties(mbedcrypto PROPERTIES VERSION 2.6.0 SOVERSION 0)
    target_link_libraries(mbedcrypto ${libs})

    add_library(mbedx509 SHARED ${src_x509})
    set_target_properties(mbedx509 PROPERTIES VERSION 2.6.0 SOVERSION 0)
    target_link_libraries(mbedx509 ${libs} mbedcrypto)

    add_library(mbedtls SHARED ${src_tls})
    set_target_properties(mbedtls PROPERTIES VERSION 2.6.0 SOVERSION 10)
    target_link_libraries(mbedtls ${libs} mbedx509)
endif (USE_SHARED_MBEDTLS_LIBRARY)

# Create custom target for MbedTLS libraries
add_custom_target(lib DEPENDS mbedcrypto mbedx509 mbedtls)
if (USE_STATIC_MBEDTLS_LIBRARY AND USE_SHARED_MBEDTLS_LIBRARY)
    add_dependencies(lib mbedcrypto_static mbedx509_static mbedtls_static)
endif ()

add_library(xhook SHARED IMPORTED)
set_target_properties(xhook
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libxhook.so
        IMPORTED_NO_SONAME TRUE  # 禁止携带路径
)

# Build main mglogger library
add_library(mglogger SHARED
        ${SOURCE_FILES}
        jni/mglogger_jni.c
)
target_link_libraries(mglogger
        xhook
        ${log-lib}
        z
        ${mbedcrypto_static_target}
)

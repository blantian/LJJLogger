#include "AesUtil.h"
#include "mbedtls/aes.h"
#include <cstring>
namespace mg {
    AesUtil::AesUtil() {
        std::memset(mKey, 0, sizeof(mKey));
        std::memset(mIv, 0, sizeof(mIv));
    }

    void AesUtil::initKeyIv(const unsigned char *key, const unsigned char *iv) {
        std::memcpy(mKey, key, 16);
        std::memcpy(mIv, iv, 16);
    }

    void
    AesUtil::encrypt(const unsigned char *in, unsigned char *out, int length, unsigned char *iv) {
        mbedtls_aes_context context;
        mbedtls_aes_init(&context);
        mbedtls_aes_setkey_enc(&context, mKey, 128);
        mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_ENCRYPT, length, iv, in, out);
        mbedtls_aes_free(&context);
    }

    void AesUtil::copyIv(unsigned char *out) const {
        std::memcpy(out, mIv, 16);
    }
}

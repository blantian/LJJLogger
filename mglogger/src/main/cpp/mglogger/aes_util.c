#include "aes_util.h"
#include "mbedtls/aes.h"

static unsigned char KEY[16] = {0};
static unsigned char IV[16] = {0};

void aes_encrypt_logger(unsigned char *in, unsigned char *out, int length, unsigned char *iv) {
    mbedtls_aes_context context;
    mbedtls_aes_setkey_enc(&context, (unsigned char *) KEY, 128);
    mbedtls_aes_crypt_cbc(&context, MBEDTLS_AES_ENCRYPT, length, iv, in, out); //加密
}

void aes_init_key_iv(const char *key, const char *iv) {
    memcpy(KEY, key, 16);
    memcpy(IV, iv, 16);
}

void aes_inflate_iv_logger(unsigned char *aes_iv) {
    memcpy(aes_iv, IV, 16);
}

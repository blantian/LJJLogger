

#ifndef CLOGAN_AES_UTIL_H
#define CLOGAN_AES_UTIL_H

#include <string.h>

void aes_encrypt_logger(unsigned char *in, unsigned char *out, int length, unsigned char *iv);

void aes_init_key_iv(const char *key, const char *iv);

void aes_inflate_iv_logger(unsigned char *aes_iv);

#endif //CLOGAN_AES_UTIL_H

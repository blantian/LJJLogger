#ifndef MGLOGGER_MG_AESUTIL_H
#define MGLOGGER_MG_AESUTIL_H

#include <cstddef>
namespace mg {
    class AesUtil {
    public:
        AesUtil();

        void initKeyIv(const unsigned char *key, const unsigned char *iv);

        void encrypt(const unsigned char *in, unsigned char *out, int length, unsigned char *iv);

        void copyIv(unsigned char *out) const;

    private:
        unsigned char mKey[16]{0};
        unsigned char mIv[16]{0};
    };
}
#endif // MGLOGGER_MG_AESUTIL_H

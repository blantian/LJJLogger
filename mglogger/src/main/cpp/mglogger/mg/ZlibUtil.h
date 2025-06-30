#ifndef MGLOGGER_MG_ZLIBUTIL_H
#define MGLOGGER_MG_ZLIBUTIL_H

#include <memory>
#include <vector>
#include <zlib.h>
#include "AesUtil.h"

#define LOGAN_CHUNK 16384

namespace mg {

    class ZlibUtil {
    public:
        explicit ZlibUtil(std::shared_ptr<AesUtil> aes);
        ~ZlibUtil();

        bool init();
        std::vector<unsigned char> compress(const unsigned char *data, int length, int type);
        std::vector<unsigned char> endCompress();
        void reset();

    private:
        void handleData(const unsigned char *data, int length, std::vector<unsigned char> &out);

        std::shared_ptr<AesUtil> mAes;
        struct ZStreamDeleter {
            void operator()(z_stream *s) const noexcept {
                if (s) {
                    deflateEnd(s);
                    delete s;
                }
            }
        };
        std::unique_ptr<z_stream, ZStreamDeleter> mStream;
        bool mReady{false};
        unsigned char mRemain[16]{0};
        int mRemainLen{0};
    };

} // namespace mg

#endif // MGLOGGER_MG_ZLIBUTIL_H
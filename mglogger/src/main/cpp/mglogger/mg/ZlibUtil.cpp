#include "ZlibUtil.h"
#include <cstring>

namespace mg {

ZlibUtil::ZlibUtil(std::shared_ptr<AesUtil> aes)
        : mAes(std::move(aes)), mStream(new z_stream{}, [](z_stream *s) {
    if (s) {
        deflateEnd(s);
        delete s;
    }
}) {
    std::memset(mRemain, 0, sizeof(mRemain));
}

ZlibUtil::~ZlibUtil() = default;

bool ZlibUtil::init() {
    if (!mStream) {
        mStream = std::unique_ptr<z_stream>(new z_stream{}, [](z_stream *s) {
            if (s) {
                deflateEnd(s);
                delete s;
            }
        });
    }
    std::memset(mStream.get(), 0, sizeof(z_stream));
    int ret = deflateInit2(mStream.get(), Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16,
                           8, Z_DEFAULT_STRATEGY);
    mReady = ret == Z_OK;
    return mReady;
}

void ZlibUtil::handleData(const unsigned char *data, int length, std::vector<unsigned char> &out) {
    int total = mRemainLen + length;
    int block = (total / 16) * 16;
    int remain = total % 16;
    if (block > 0) {
        std::vector<unsigned char> buffer(block);
        unsigned char *ptr = buffer.data();
        if (mRemainLen) {
            std::memcpy(ptr, mRemain, mRemainLen);
            ptr += mRemainLen;
        }
        int copy_len = block - mRemainLen;
        if (copy_len > 0 && data) {
            std::memcpy(ptr, data, copy_len);
        }
        unsigned char iv[16];
        mAes->copyIv(iv);
        std::vector<unsigned char> enc(block);
        mAes->encrypt(buffer.data(), enc.data(), block, iv);
        out.insert(out.end(), enc.begin(), enc.end());
        if (length > copy_len) {
            data += copy_len;
        }
    }
    if (remain > 0) {
        const unsigned char *remain_src = data + (block - mRemainLen);
        std::memcpy(mRemain, remain_src, remain);
    }
    mRemainLen = remain;
}

std::vector<unsigned char> ZlibUtil::compress(const unsigned char *data, int length, int type) {
    std::vector<unsigned char> encrypted;
    if (!mAes) return encrypted;

    if (mReady) {
        mStream->avail_in = static_cast<uInt>(length);
        mStream->next_in = const_cast<unsigned char *>(data);
        unsigned char out[LOGAN_CHUNK];
        int ret;
        do {
            mStream->avail_out = LOGAN_CHUNK;
            mStream->next_out = out;
            ret = deflate(mStream.get(), type);
            if (ret == Z_STREAM_ERROR) {
                mReady = false;
                break;
            }
            unsigned have = LOGAN_CHUNK - mStream->avail_out;
            if (have > 0) {
                handleData(out, have, encrypted);
            }
        } while (mStream->avail_out == 0);
    } else if (length > 0) {
        handleData(data, length, encrypted);
    }
    return encrypted;
}

std::vector<unsigned char> ZlibUtil::endCompress() {
    std::vector<unsigned char> out;
    if (mReady) {
        std::vector<unsigned char> tmp = compress(nullptr, 0, Z_FINISH);
        out.insert(out.end(), tmp.begin(), tmp.end());
        reset();
    }
    unsigned char padding[16];
    int val = 16 - mRemainLen;
    std::memset(padding, val, 16);
    if (mRemainLen > 0) {
        std::memcpy(padding, mRemain, mRemainLen);
    }
    unsigned char iv[16];
    mAes->copyIv(iv);
    std::vector<unsigned char> enc(16);
    mAes->encrypt(padding, enc.data(), 16, iv);
    out.insert(out.end(), enc.begin(), enc.end());
    mRemainLen = 0;
    return out;
}

void ZlibUtil::reset() {
    if (mStream) {
        deflateEnd(mStream.get());
    }
    mReady = false;
    mRemainLen = 0;
}

} // namespace mg


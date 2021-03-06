#pragma once

#include <setjmp.h>
#include <map>
#include <type_traits>
#include <vector>

#include "image.h"
#include <jpeglib.h>
#include "params.h"

namespace quink {

class ImageEncoder {
public:
    virtual ~ImageEncoder() = default;
    virtual int EncodeFloat(const std::string &file, std::shared_ptr<Image<float>> img);
    virtual int EncodeUInt8(const std::string &file, std::shared_ptr<Image<uint8_t>> img) { return -1; }

    virtual std::string GetDefaultSuffix() { return std::string(); }

    bool SetInt(const std::string &key, int value);

protected:
    Params mConfig;
};

class PngEncoder : public ImageEncoder {
public:
    int EncodeUInt8(const std::string &file, std::shared_ptr<Image<uint8_t>> img) override;

    std::string GetDefaultSuffix() override { return ".png"; }
};

class JpegEncoder : public ImageEncoder {
public:
    JpegEncoder(int quality = 95);

    int EncodeUInt8(const std::string &file, std::shared_ptr<Image<uint8_t>> img) override;
    std::string GetDefaultSuffix() override { return ".jpg"; }

private:
    static void JpegErrorExit(j_common_ptr);
    static void JpegErrorMessage(j_common_ptr);

    int mQuality;
    jpeg_error_mgr mErrorMgr;
    jmp_buf mJmpBuffer;
};

class PairEncoder : public ImageEncoder {
public:
    static const uint8_t magic_numer = 30;
    PairEncoder(std::shared_ptr<ImageEncoder> enc);

    int EncodeFloat(const std::string &file, std::shared_ptr<Image<float>> img) override;
    std::string GetDefaultSuffix() override;

private:
    std::shared_ptr<ImageEncoder> mEncoder;
};

class ImageStore {
public:
    ImageStore() = delete;
    ~ImageStore() = delete;

    static int StoreImage(const std::string &file, ImageFormat format, std::shared_ptr<Image<float>> img) {
        std::unique_ptr<ImageEncoder> enc = GetEncoder(format);
        if (enc) {
            return enc->EncodeFloat(file, img);
        } else {
            return -1;
        }
    }

    static int StoreImage(const std::string &file, ImageFormat format, std::shared_ptr<Image<uint8_t>> img) {
        std::unique_ptr<ImageEncoder> enc = GetEncoder(format);
        if (enc) {
            return enc->EncodeUInt8(file, img);
        } else {
            return -1;
        }
    }

    static std::unique_ptr<ImageEncoder> GetEncoder(ImageFormat format) {
        std::unique_ptr<ImageEncoder> enc;
        switch (format)
        {
        case ImageFormat::PngImage:
            enc.reset(new PngEncoder());
            break;
        case ImageFormat::JpegImage:
            enc.reset(new JpegEncoder());
            break;
        }
        return enc;
    }

    static std::vector<std::shared_ptr<ImageEncoder>> GetEncoders() {
        std::vector<std::shared_ptr<ImageEncoder>> enc {
            std::shared_ptr<ImageEncoder>(new PngEncoder()),
            std::shared_ptr<ImageEncoder>(new JpegEncoder()),
        };

        return enc;
    }
};

} // namespace quink

#pragma once

#include "image_decoder.h"

namespace quink {

class OpenEXRDecoder : public ImageDecoder {
public:
    ImageWrapper Decode(const std::string &file) override;
};

class OpenEXRDecoderFactory : public ImageDecoderFactory {
public:
    int Probe(const std::string &file) const override;
    std::unique_ptr<ImageDecoder> Create() const override;
    std::string GetDecoderName() const override { return "OpenEXRDecoder"; }
};

}

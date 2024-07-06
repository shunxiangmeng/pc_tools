#pragma once
#include "hal/Image.h"

namespace hal {
class x86Image : public IImage {
    x86Image() = default;
    virtual ~x86Image() = default;
public:
    static x86Image* instance();
    virtual bool setInputFramerate(int32_t channel, uint32_t fps) override;
    virtual int getInputFramerate(int32_t channel) override;
};
}

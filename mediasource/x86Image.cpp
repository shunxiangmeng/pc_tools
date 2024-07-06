
#include "x86Image.h"

namespace hal {

IImage* IImage::instance() {
    return x86Image::instance();
}
x86Image* x86Image::instance() {
    static x86Image s_image;
    return &s_image;
}  

bool x86Image::setInputFramerate(int32_t channel, uint32_t fps) {
    return false;
}

int x86Image::getInputFramerate(int32_t channel) {
    return 0;
}

}
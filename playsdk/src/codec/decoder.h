/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  decoder.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 13:26:41
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "infra/include/thread/Thread.h"
#include "common/mediaframe/MediaFrame.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}

namespace playsdk {

class Decoder {
public:
    Decoder();
    ~Decoder();

    bool init();

    bool inputMediaFrame(MediaFrame frame);


private:
    bool initShaders();

private:
    AVCodecContext* av_codec_context_ = nullptr;
    AVCodec* av_codec_ = nullptr;
};

}
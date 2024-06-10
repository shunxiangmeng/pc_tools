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
#include "../DecodedFrame.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libswresample/swresample.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/opt.h"
}

namespace playsdk {

class Decoder {
public:
    Decoder(DecodedFrameList &decoded_frame_queue);
    ~Decoder();

    bool init(VideoFrameInfo videoinfo);
    bool init(AudioFrameInfo audioinfo);

    bool inputMediaFrame(MediaFrame frame);


private:
    

private:
    MediaFrameType video_audio_;
    AVCodecContext* av_codec_context_ = nullptr;
    AVCodec* av_codec_ = nullptr;
    SwrContext* swr_context_ = nullptr;
    AVFrame *convertframe_ = nullptr;

    DecodedFrameList& decoded_frame_queue_;
};

}
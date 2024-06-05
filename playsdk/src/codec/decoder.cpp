/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  decoder.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 13:26:26
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "decoder.h"
#include "infra/include/Logger.h"
namespace playsdk {

Decoder::Decoder() {
}
Decoder::~Decoder() {
}

bool Decoder::init() {
    av_codec_context_ = avcodec_alloc_context3(NULL);
    av_codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (av_codec_ == nullptr) {
        return false;
    }
    av_codec_context_->time_base.num = 1;
    av_codec_context_->time_base.den = 25;
    av_codec_context_->frame_number = 1;
    av_codec_context_->codec_type = AVMEDIA_TYPE_VIDEO;
    av_codec_context_->bit_rate = 0;
    av_codec_context_->width = 0;
    av_codec_context_->height = 0;
    av_codec_context_->pix_fmt = AV_PIX_FMT_YUVJ420P;
    av_codec_context_->color_range = AVCOL_RANGE_MPEG;
    if (avcodec_open2(av_codec_context_, av_codec_, NULL) < 0) {
        errorf("open decoder error\n");
        return false;
    }
    infof("init decoder succeed\n");
    return true;
};

bool Decoder::inputMediaFrame(MediaFrame frame) {
    AVPacket packet = { 0 };
    packet.data = (uint8_t*)frame.data();
    packet.size = frame.size();
    int ret = avcodec_send_packet(av_codec_context_, &packet);
    if (ret < 0) {
        errorf("decode failed ret:%d\n", ret);
    }
    AVFrame *yuv_frame = av_frame_alloc();
    if (avcodec_receive_frame(av_codec_context_, yuv_frame) == 0) {
        int width = av_codec_context_->width;
        int height = av_codec_context_->height;
        
        infof("video width:%d, height:%d\n", width, height);
    }

    return true;
}

}
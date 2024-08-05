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
#include "infra/include/Timestamp.h"

namespace playsdk {

Decoder::Decoder(DecodedFrameList& decoded_frame_queue) : initailed_(false), video_audio_(InvalidFrameType), decoded_frame_queue_(decoded_frame_queue) {
}
Decoder::~Decoder() {
}

bool Decoder::init(VideoFrameInfo videoinfo) {
    video_audio_ = MediaFrameType::Video;
    AVCodecID ff_codec;
    switch (videoinfo.codec) {
    case H264: ff_codec = AV_CODEC_ID_H264; break;
    case H265: ff_codec = AV_CODEC_ID_H265; break;
    default:
        errorf("not support video codec type %d\n", videoinfo.codec);
        return false;
    }
    av_codec_context_ = avcodec_alloc_context3(NULL);
    av_codec_ = avcodec_find_decoder(ff_codec);
    if (av_codec_ == nullptr) {
        errorf("avcodec_find_decoder failed\n");
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
    initailed_ = true;
    infof("init decoder succeed\n");
    return true;
}

bool Decoder::init(AudioFrameInfo audioinfo) {
    video_audio_ = MediaFrameType::Audio;
    AVCodecID ff_codec;
    switch (audioinfo.codec) {
    case AAC: ff_codec = AV_CODEC_ID_AAC; break;
    case G711a: ff_codec = AV_CODEC_ID_PCM_ALAW; break;
    case G711u: ff_codec = AV_CODEC_ID_PCM_MULAW; break;
    default:
        errorf("not support audio codec type %d\n", audioinfo.codec);
        return false;
    }
    av_codec_context_ = avcodec_alloc_context3(NULL);
    av_codec_ = avcodec_find_decoder(ff_codec);
    if (av_codec_ == nullptr) {
        errorf("avcodec_find_decoder failed\n");
        return false;
    }

    av_codec_context_->codec_type = AVMEDIA_TYPE_AUDIO;
    av_codec_context_->sample_rate = audioinfo.sample_rate;
    av_codec_context_->channels = audioinfo.channel_count;
    if (av_codec_context_->channels > 1) {
        av_codec_context_->channel_layout = AV_CH_LAYOUT_STEREO;
    } else {
        av_codec_context_->channel_layout = AV_CH_LAYOUT_MONO;
    }
    av_codec_context_->sample_fmt = AV_SAMPLE_FMT_S16;

    if (avcodec_open2(av_codec_context_, av_codec_, NULL) < 0) {
        errorf("open decoder error\n");
        return false;
    }
    initailed_ = true;
    infof("init decoder succeed\n");
    return true;
}

bool Decoder::running() const {
    return initailed_;
}

bool Decoder::inputMediaFrame(MediaFrame frame) {
    if (!initailed_ || frame.empty() || decoded_frame_queue_.size() > 2) {
        return false;
    }

    AVPacket packet = { 0 };
    packet.data = (uint8_t*)frame.data();
    packet.size = frame.size();
    packet.dts = frame.dts();
    packet.pts = frame.pts();
    int ret = avcodec_send_packet(av_codec_context_, &packet);
    if (ret < 0) {
        errorf("decode failed ret:%d\n", ret);
        return false;
    }
    AVFrame *decoded_frame = av_frame_alloc();
    if (avcodec_receive_frame(av_codec_context_, decoded_frame) == 0) {
        if (video_audio_ == MediaFrameType::Audio) {
            int64_t inout_ch_layout = AV_CH_LAYOUT_MONO;
            if (decoded_frame->channels > 1) {
                inout_ch_layout = AV_CH_LAYOUT_STEREO;
            }

            if (swr_context_ == nullptr) {
                swr_context_ = swr_alloc_set_opts(nullptr,
                    inout_ch_layout, (AVSampleFormat)AV_SAMPLE_FMT_S16, decoded_frame->sample_rate,
                    inout_ch_layout, (AVSampleFormat)decoded_frame->format, decoded_frame->sample_rate,
                    0, nullptr);
                if (swr_context_== nullptr) {
                    errorf("swr_alloc_set_opts error, ret:%d\n", ret);
                    return false;
                }
                swr_init(swr_context_);
            }

            int convert_line_size = 0;
            int out_buffer_size = av_samples_get_buffer_size(&convert_line_size, decoded_frame->channels, decoded_frame->nb_samples, AV_SAMPLE_FMT_S16, 0);
            uint8_t *out_buffer = (uint8_t *)av_malloc(out_buffer_size);
            if (out_buffer == nullptr) {
                errorf("av_malloc %d failed\n", out_buffer_size);
                return false;
            }
            AVFrame *convertframe = av_frame_alloc();
            convertframe->nb_samples = decoded_frame->nb_samples;
            convertframe->format = AV_SAMPLE_FMT_S16;
            convertframe->channels = decoded_frame->channels;
            convertframe->pts = decoded_frame->pts;
            convertframe->sample_rate = decoded_frame->sample_rate;

            ret = avcodec_fill_audio_frame(convertframe, decoded_frame->channels, AV_SAMPLE_FMT_S16, (const uint8_t*)out_buffer, out_buffer_size, 0);
            if (ret < 0) {
                errorf("avcodec_fill_audio_frame error. ret:%d\n", ret);
                av_frame_free(&convertframe);
                av_free(out_buffer);
                return false;
            }

            swr_convert(swr_context_, (uint8_t**)convertframe->data, decoded_frame->nb_samples, (const uint8_t**)decoded_frame->data, decoded_frame->nb_samples);
            
            int32_t per_sample_bytes = av_get_bytes_per_sample(av_codec_context_->sample_fmt);
            //infof("frame size:%d, pts:%lld, channels:%d, samples:%d, rate:%d, per_sample_bytes:%d\n", 
            //    decoded_frame->linesize[0], decoded_frame->pts, decoded_frame->channels, decoded_frame->nb_samples, decoded_frame->sample_rate, per_sample_bytes);
        
            DecodedFrame aframe(convertframe);
            decoded_frame_queue_.push(aframe);
            av_frame_free(&convertframe);
            //infof("audio decoded_frame_size:%d\n", decoded_frame_queue_.size());
        } else {
            int width = av_codec_context_->width;
            int height = av_codec_context_->height;
            //infof("video width:%d, height:%d, pts:%lld\n", width, height, decoded_frame->pkt_pts);
            DecodedFrame vframe(decoded_frame);
            decoded_frame_queue_.push(vframe);
            //infof("video decoded_frame_size:%d\n", decoded_frame_queue_.size());
        }
    }
    av_frame_free(&decoded_frame);
    return true;
}

}

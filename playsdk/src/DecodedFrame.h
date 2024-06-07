/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  DecodecFrame.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-07 11:03:12
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <mutex>
#include <queue>
#include <memory>

extern "C" {
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}

namespace playsdk {

class DecodedFrame {
public:
    DecodedFrame();
    ~DecodedFrame();
    DecodedFrame(const DecodedFrame& other);
    DecodedFrame(DecodedFrame&& other);
    DecodedFrame(AVFrame *avframe);
    void operator=(const DecodedFrame& other);
public:
    AVFrame *frame_;
};


class DecodedFrameList {
public:
    DecodedFrameList();
    ~DecodedFrameList();

    int32_t size();
    void push(DecodedFrame frame);
    void pop();
    DecodedFrame front();
private:
    std::recursive_mutex mutex_;
    std::queue<DecodedFrame> decoded_frame_queue_;
};

}
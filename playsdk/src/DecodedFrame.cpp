/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  DecodedFrame.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-07 11:09:38
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "DecodedFrame.h"

namespace playsdk {

DecodedFrame::DecodedFrame() {
    frame_ = av_frame_alloc();
}

DecodedFrame::DecodedFrame(AVFrame* avframe) {
    frame_ = av_frame_alloc();
    av_frame_ref(frame_, avframe);
}

DecodedFrame::~DecodedFrame() {
    if (frame_) {
        av_frame_unref(frame_);
        av_frame_free(&frame_);
    }
}

DecodedFrame::DecodedFrame(const DecodedFrame& other) {
    frame_ = av_frame_alloc();
    av_frame_ref(frame_, other.frame_);
}

DecodedFrame::DecodedFrame(DecodedFrame&& other) {
    frame_ = av_frame_alloc();
    av_frame_ref(frame_, other.frame_);
    av_frame_unref(other.frame_);
}

void DecodedFrame::operator=(const DecodedFrame& other) {
    av_frame_ref(frame_, other.frame_);
}


DecodedFrameList::DecodedFrameList() {
}

DecodedFrameList::~DecodedFrameList() {
}

int32_t DecodedFrameList::size() {
    return (int32_t)decoded_frame_queue_.size();
}

void DecodedFrameList::push(DecodedFrame frame) {
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    decoded_frame_queue_.push(frame);
}
void DecodedFrameList::pop() {
    if (decoded_frame_queue_.size() == 0) {
        return;
    }
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    decoded_frame_queue_.pop();
}

DecodedFrame DecodedFrameList::front() {
    if (decoded_frame_queue_.size() == 0) {
        return DecodedFrame();
    }
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    DecodedFrame frame = decoded_frame_queue_.front();
    return frame;
}

}
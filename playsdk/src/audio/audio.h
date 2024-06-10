/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  audio.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-09 22:43:54
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <memory>
#include "aldlist.h"
#include "LoadOAL.h"
#include "infra/include/thread/Thread.h"
#include "../DecodedFrame.h"

namespace playsdk {

#define NUMBUFFERS              (4)

class Audio : public infra::Thread {
public:
    Audio(DecodedFrameList& decoded_frame_queue);
    ~Audio();
    bool initialize();
    void deInitialize();
private:
    virtual void run() override;

    int32_t putFrame(DecodedFrame frame);

private:
    std::shared_ptr<ALDeviceList> al_device_list_;
    DecodedFrameList& decoded_frame_queue_;

    bool playing_ = false;
    ALenum output_format_ = 0;
    ALuint al_buffers_[NUMBUFFERS];
    ALuint al_source_id_;
    ALint al_buffers_processed_ = 0;
    ALint al_total_buffers_processed_ = 0;
    ALint al_queued_buffers_ = 0;
};

}
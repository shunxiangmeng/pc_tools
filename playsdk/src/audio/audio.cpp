#/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  audio.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-09 22:44:21
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "Audio.h"
#include "infra/include/Logger.h"
#include "infra/include/Timestamp.h"
#include "infra/include/Buffer.h"
#include "OpenAL/include/al.h"
#include "OpenAL/include/alc.h"
#include "OpenAL/include/efx.h"
#include "OpenAL/include/efx-creative.h"
#include "OpenAL/include/xram.h"

namespace playsdk {

Audio::Audio(DecodedFrameList& decoded_frame_queue) : decoded_frame_queue_(decoded_frame_queue) {
}

Audio::~Audio() {
}

bool Audio::initialize() {
    al_device_list_ = std::make_shared<ALDeviceList>();
    ALint i;
    for (i = 0; i < al_device_list_->GetNumDevices(); i++) {
        infof("%d. %s%s\n", i + 1, al_device_list_->GetDeviceName(i), i == al_device_list_->GetDefaultDevice() ? "(DEFAULT)" : "");
    }
    
    ALCdevice *device = alcOpenDevice(al_device_list_->GetDeviceName(i - 1));
    if (device) {
        ALCcontext *context = alcCreateContext(device, NULL);
        if (context) {
            infof("Opened %s Device\n", alcGetString(device, ALC_DEVICE_SPECIFIER));
            alcMakeContextCurrent(context);
        } else {
            alcCloseDevice(device);
        }
    }

    output_format_ = AL_FORMAT_STEREO16;
    alGenBuffers(NUMBUFFERS, al_buffers_);
    alGenSources(1, &al_source_id_);
    //alSourcei(al_source_id_, AL_BUFFER, al_buffer_id_);
    //alSourcePlay(al_source_id_);

    infra::Thread::start();
    return true;
}

void Audio::deInitialize() {
    ALCcontext *context = alcGetCurrentContext();
    ALCdevice *device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

int32_t Audio::putFrame(DecodedFrame frame) {
    int32_t need_frame_count = 0;
    if (!playing_) {
        infra::Buffer silence(frame.frame_->linesize[0]);
        silence.resize(frame.frame_->linesize[0]);
        memset(silence.data(), 0x00, silence.size());
        int i = 0;
        for (; i < NUMBUFFERS - 1; i++) {
            alBufferData(al_buffers_[i], output_format_, silence.data(), silence.size(), frame.frame_->sample_rate);
            alSourceQueueBuffers(al_source_id_, 1, &al_buffers_[i]);
        }
        alBufferData(al_buffers_[i], output_format_, frame.frame_->data[0], frame.frame_->linesize[0], frame.frame_->sample_rate);
        alSourceQueueBuffers(al_source_id_, 1, &al_buffers_[i]);

        alSourcePlay(al_source_id_);
        playing_ = true;
        al_total_buffers_processed_ = 0;
    } else {
        alGetSourcei(al_source_id_, AL_BUFFERS_PROCESSED, &al_buffers_processed_);
        al_total_buffers_processed_ += al_buffers_processed_;
        //tracef("al_buffers_processed_:%d\n", al_buffers_processed_);
        if (al_buffers_processed_) {
            ALuint buffer;
            int32_t play_sample_rate = frame.frame_->sample_rate;
            play_sample_rate = 50000;

            alSourceUnqueueBuffers(al_source_id_, 1, &buffer);
            alBufferData(buffer, output_format_, frame.frame_->data[0], frame.frame_->linesize[0], play_sample_rate);
            alSourceQueueBuffers(al_source_id_, 1, &buffer);
        }
        ALint state;
        alGetSourcei(al_source_id_, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            alGetSourcei(al_source_id_, AL_BUFFERS_QUEUED, &al_queued_buffers_);
            if (al_queued_buffers_) {
                warnf("source play......\n");
                alSourcePlay(al_source_id_);
            }
        }
        need_frame_count = al_buffers_processed_ - 1;
    }
    return need_frame_count;
}

void Audio::run() {
    warnf("audio thread start\n");
    while (running()) {
        if (decoded_frame_queue_.size()) {
            
            int32_t need_frame = 0;
            do {
                DecodedFrame frame = decoded_frame_queue_.front();
                need_frame = putFrame(frame);
                if (need_frame >= 0) {
                    decoded_frame_queue_.pop();
                }
            } while (need_frame > 0);
            //int64_t now = infra::getCurrentTimeMs();
            //debugf("audio pts:%lld, now:%lld\n", frame.frame_->pts, now);
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    warnf("audio thread exit\n");
}


}
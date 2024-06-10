/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  playsdk.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:04:14
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "playsdk.h"
#include "infra/include/Logger.h"

namespace playsdk {

std::shared_ptr<IPlaysdk> IPlaysdk::create() {
    return std::make_shared<Playsdk>();
}

Playsdk::Playsdk() : playmode_(PlayModeLive) {
    video_decoder_ = std::make_shared<Decoder>(video_decoded_frame_queue_);
    audio_decoder_ = std::make_shared<Decoder>(audio_decoded_frame_queue_);
    render_ = std::make_shared<Render>(video_decoded_frame_queue_);
    audio_ = std::make_shared<Audio>(audio_decoded_frame_queue_);
}

Playsdk::~Playsdk() {
}

bool Playsdk::init(PlayMode playmode) {
    playmode_ = playmode;
    if (!audio_->initialize()) {
        return false;
    }
    if (!render_->initial()) {
        return false;
    }
    return true;
}

bool Playsdk::inputMediaFrame(MediaFrame frame) {
    if (playmode_ == PlayModeFile) {
        return false;
    }
    return true;
}

bool Playsdk::setMediaFileName(const char* filename) {
    if (playmode_ == PlayModeLive) {
        return false;
    }
    if (!mp4_reader_) {
        mp4_reader_ = std::make_shared<MP4Reader>();
    }
    mp4_filename_ = filename;
    return mp4_reader_->open(filename);
}

bool Playsdk::start() {
    if (playmode_ == PlayModeFile) {
        return startPlayfile();
    }
    return true;
}



bool Playsdk::startPlayfile() {
    infof("start playfile %s\n", mp4_filename_.data());
    VideoFrameInfo videoinfo;
    AudioFrameInfo audioinfo;
    mp4_reader_->getVideoInfo(videoinfo);
    mp4_reader_->getAudioInfo(audioinfo);
    if (!video_decoder_->init(videoinfo)) {
        return false;
    }
    if (!audio_decoder_->init(audioinfo)) {
        return false;
    }

    std::thread([this]() {
        infof("start playfile thread\n");
        while (true) {
            MediaFrame frame = mp4_reader_->getFrame();
            if (!frame.empty()) {
                if (frame.getMediaFrameType() == MediaFrameType::Video) {
                    //tracef("encoded frame pts:%lld\n", frame.pts());
                    frame.convertPlacementTypeToAnnexb();
                    video_encoded_frame_queue_.push_back(frame);
                }
                if (frame.getMediaFrameType() == MediaFrameType::Audio) {
                    audio_encoded_frame_queue_.push_back(frame);
                }
            }
            MediaFrame to_decoded_frame = video_encoded_frame_queue_.front();
            if (video_decoder_->inputMediaFrame(to_decoded_frame)) {
                video_encoded_frame_queue_.pop_front();
            }

            MediaFrame to_decoded_audio_frame = audio_encoded_frame_queue_.front();
            if (audio_decoder_->inputMediaFrame(to_decoded_audio_frame)) {
                audio_encoded_frame_queue_.pop_front();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        infof("exit playfile thread\n");
    }).detach();
    return true;
}

}
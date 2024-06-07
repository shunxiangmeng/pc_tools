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
    decoder_ = std::make_shared<Decoder>(video_decoded_frame_queue_);
    render_ = std::make_shared<Render>(video_decoded_frame_queue_);
}

Playsdk::~Playsdk() {
}

bool Playsdk::init(PlayMode playmode) {
    playmode_ = playmode;
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
    mp4_reader_->getVideoInfo(videoinfo);

    if (!decoder_->init(videoinfo.codec)) {
        return false;
    }

    std::thread([this]() {
        infof("start playfile thread\n");
        while (true) {
            MediaFrame frame = mp4_reader_->getFrame();
            if (!frame.empty()) {
                if (frame.getMediaFrameType() == Video) {
                    frame.convertPlacementTypeToAnnexb();
                    decoder_->inputMediaFrame(frame);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        infof("exit playfile thread\n");
    }).detach();
    return true;
}

}
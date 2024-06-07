/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  playsdk.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:04:49
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <memory>
#include "playsdk/include/IPlaysdk.h"
#include "codec/decoder.h"
#include "render/render.h"
#include "DecodedFrame.h"
#include "common/mediafiles/mp4/MP4Reader.h"

namespace playsdk {

class Playsdk : public IPlaysdk {
public:
    Playsdk();
    virtual ~Playsdk();

    virtual bool init(PlayMode playmode) override;
    virtual bool inputMediaFrame(MediaFrame frame) override;
    virtual bool setMediaFileName(const char* filename) override;
    virtual bool start() override;

private:
    bool startPlayfile();

private:
    PlayMode playmode_;
    std::shared_ptr<Decoder> decoder_;
    std::shared_ptr<Render> render_;

    std::shared_ptr<MP4Reader> mp4_reader_;
    std::string mp4_filename_;

    DecodedFrameList video_decoded_frame_queue_;
    DecodedFrameList audio_decoded_frame_queue_;

};
    
}
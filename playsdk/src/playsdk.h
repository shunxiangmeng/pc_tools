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
#include "audio/audio.h"
#include "DecodedFrame.h"
#include "Client.h"
#include "common/mediafiles/mp4/MP4Reader.h"
#include "common/mediaframe/MediaFrameList.h"
#include "infra/include/RateStatistics.h"
#include "infra/include/Timer.h"

namespace playsdk {

class Playsdk : public IPlaysdk {
public:
    Playsdk();
    virtual ~Playsdk();

    virtual bool init(PlayMode playmode) override;
    virtual void setClient(std::weak_ptr<IClient> client) override;
    virtual bool inputMediaFrame(MediaFrame frame) override;
    virtual bool setMediaFileName(const char* filename) override;
    virtual bool start() override;
    virtual bool setSpeed(float speed) override;
    virtual float speed() override;
    virtual bool setTrackingBox(Json::Value &data) override;
    virtual bool setDetectRegion(Json::Value& data) override;
    virtual float getVideoBitrate() override;

private:
    bool startPlayfile();
    bool startPlayStream();

private:
    PlayMode playmode_;
    bool running_;
    std::shared_ptr<Decoder> video_decoder_;
    std::shared_ptr<Decoder> audio_decoder_;
    std::shared_ptr<Render> render_;
    std::shared_ptr<Audio> audio_;
    
    std::shared_ptr<MP4Reader> mp4_reader_;
    std::string mp4_filename_;

    MediaFrameList video_encoded_frame_queue_;
    MediaFrameList audio_encoded_frame_queue_;

    DecodedFrameList video_decoded_frame_queue_;
    DecodedFrameList audio_decoded_frame_queue_;

    float speed_ = 1.0f;
    infra::RateStatistics video_rate_statistics_;
    infra::Timer rate_timer_;
};
    
}
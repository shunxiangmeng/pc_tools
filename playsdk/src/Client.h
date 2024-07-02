/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Client.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-02 09:37:55
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "playsdk/include/IClient.h"
#include "private/include/IPrivClient.h"
#include "playsdk/include/IPlaysdk.h"

namespace playsdk {

class Client : public IClient {
public:
    Client();
    ~Client();
    virtual bool init() override;
    virtual void interaction(void* window) override;
    bool init(std::string server_host, uint16_t port);

    void initInteraction();

private:
    bool subscribeEvent();
    void onMediaFrame(MediaFrameType type, MediaFrame& frame);

    void interaction_tab_video();

private:
    std::string server_ip_;
    uint16_t server_port_;
    std::shared_ptr<IPrivClient> client_;
    std::shared_ptr<IPlaysdk> playsdk_;
    std::shared_ptr<IPrivClient> event_client_;


    bool show_demo_window_ = true;
    std::vector<char*> resolution_;
    std::vector<char*> bitrate_type_;
    std::vector<char*> video_codec_type_;

    int32_t main_resolution_index_ = 1;
    int32_t main_bitrate_type_ = 0;
    int32_t main_video_encode_type_index_ = 0;

    int32_t sub_resolution_index_ = 3;
    int32_t sub_bitrate_type_ = 0;
    int32_t sub_video_encode_type_index_ = 0;
};

}
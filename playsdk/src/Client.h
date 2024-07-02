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

    void login();
    void logout();

    void interaction_tab_login();
    void interaction_tab_video();

private:
    std::string server_ip_;
    uint16_t server_port_;
    std::shared_ptr<IPrivClient> client_;
    std::shared_ptr<IPlaysdk> playsdk_;
    std::shared_ptr<IPrivClient> event_client_;

    bool show_login_window_ = true;
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

    struct VideoConfig {
        std::vector<char*> resolution_str_list;
        std::vector<char*> bitrate_str_list;
        std::vector<char*> encode_str_list;
        int32_t resolution_index;
        int32_t bitrate_type_index;
        int32_t encode_type_index;
        char fps_str[8];
        char gop_str[8];
        char bitrate_str[8];
    };

    struct InteractionLogin {
        char server_ip[32];
        char server_port[8];
        char username[32];
        char password[32];
        InteractionLogin() : server_ip("172.16.9.100"), server_port("7000"), username("admin"), password("Ulucu888") {
        }
    };

    InteractionLogin interaction_login_;
};

}
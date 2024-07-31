/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Client.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-07-02 09:38:24
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "Client.h"
#include "imgui/imgui.h"
#include "infra/include/thread/WorkThreadPool.h"

namespace playsdk {

std::shared_ptr<IClient> IClient::create() {
    return std::make_shared<Client>();
}

Client::Client() {
    client_ = IPrivClient::create();
    event_client_ = IPrivClient::create();
    playsdk_ = IPlaysdk::create();
}

Client::~Client() {
}

bool Client::init() {
    if (!playsdk_->init(playsdk::PlayModeLive)) {
        errorf("playsdk init failed\n");
        return false;
    }
    playsdk_->start();
    playsdk_->setClient(shared_from_this());
    tracef("client init succ\n");
    initInteraction();
    return true;
}

bool Client::init(std::string server_host, uint16_t port) {
    server_ip_ = server_host;
    server_port_ = port;
    return true;
}

void Client::onMediaFrame(MediaFrameType type, MediaFrame& frame) {
    //tracef("onmedia size:%d\n", frame.size());
    playsdk_->inputMediaFrame(frame);
}

bool Client::subscribeEvent() {
    uint16_t port = atoi(interaction_login_.server_port);
    if (!event_client_->connect(interaction_login_.server_ip, port)) {
        errorf("event_client connect failed\n");
        return false;
    }

    event_client_->subscribeEvent("detect_target", [this](Json::Value& data) {
        //tracef("onEvent:%s\n", data.toStyledString().data());
        playsdk_->setTrackingBox(data);
    });

    return true;
}

void Client::initInteraction() {

    video_input_format_str_list_.push_back("PAL(25fps)");
    video_input_format_str_list_.push_back("NTSC(30fps)");

    resolution_.push_back("2560x1440");
    resolution_.push_back("1920x1080");
    resolution_.push_back("1280x720");
    resolution_.push_back("640x480");

    bitrate_type_.push_back("VBR");
    bitrate_type_.push_back("CBR");

    video_codec_type_.push_back("H264");
    video_codec_type_.push_back("H265");
}

void Client::login() {
    infra::WorkThreadPool::instance()->async([this] () {
        uint16_t port = atoi(interaction_login_.server_port);
        tracef("login %s:%d \n", interaction_login_.server_ip, port);
        bool ret = client_->connect(interaction_login_.server_ip, port);
        if (!ret) {
            errorf("connect %s:%d failed\n", interaction_login_.server_ip, port);
            return;
        }
        infof("login %s:%d succ\n", interaction_login_.server_ip, port);
        onLoginSucc();
    });
}

void Client::logout() {
    tracef("logout+++\n");
}

void Client::onLoginSucc() {
    client_->startPreview(0, 0, IPrivClient::OnFrameProc(&Client::onMediaFrame, this));
    Json::Value video_config;
    client_->getVideoConfig(video_config);
    onGetVideoConfig(video_config);

    subscribeEvent();
}

void Client::onSetVideoFormat(int32_t index) {
    infra::WorkThreadPool::instance()->async([this, index] () {
        std::string video_format;
        if (index == 0) {
            video_format = "pal";
        } else if (index == 1) {
            video_format = "ntsc";
        } else {
            return;
        }
        client_->setVideoFormat(video_format);

        client_->getVideoFormat(video_format);
        if (video_format == "pal") {
            video_input_format_index_ = 0;
        } else if (video_format == "ntsc") {
            video_input_format_index_ = 1;
        }
    });
}

void Client::onGetVideoConfig(Json::Value &root) {
    if (root.isMember("format") && root["format"].isString()) {
        std::string video_format = root["format"].asString();
        if (video_format == "pal") {
            video_input_format_index_ = 0;
        } else if (video_format == "ntsc") {
            video_input_format_index_ = 1;
        }
    }

    if (!root.isMember("config") || !root["config"].isArray()) {
        return;
    }
    video_config_.clear();
    Json::Value &config = root["config"];
    for (auto &it : config) {
        VideoConfig video_config;

        video_config.encode_str_list.push_back("H264");
        video_config.encode_str_list.push_back("H265");
        std::string codec = it["codec"].asString();
        for (int i = 0; i < video_config.encode_str_list.size(); i++) {
            if (codec == video_config.encode_str_list[i]) {
                video_config.encode_type_index = i;
            }
        }

        video_config.resolution_str_list.push_back("2560x1440");
        video_config.resolution_str_list.push_back("1920x1080");
        video_config.resolution_str_list.push_back("1280x720");
        video_config.resolution_str_list.push_back("640x480");
        int32_t width = it["width"].asInt();
        int32_t height = it["height"].asInt();
        std::string resolution = std::to_string(width) + "x" + std::to_string(height);
        for (int i = 0; i < video_config.resolution_str_list.size(); i++) {
            if (resolution == video_config.resolution_str_list[i]) {
                video_config.resolution_index = i;
            }
        }

        video_config.bitrate_type_str_list.push_back("cbr");
        video_config.bitrate_type_str_list.push_back("vbr");
        std::string bitrate_type = it["bitrate_type"].asString();
        for (int i = 0; i < video_config.bitrate_type_str_list.size(); i++) {
            if (bitrate_type == video_config.bitrate_type_str_list[i]) {
                video_config.bitrate_type_index = i;
            }
        }

        int fps = it["fps"].asInt();
        snprintf(video_config.fps_str, sizeof(video_config.fps_str), "%d", fps);
        int gop = it["gop"].asInt();
        snprintf(video_config.gop_str, sizeof(video_config.gop_str), "%d", gop);
        int bitrate = it["bitrate"].asInt();
        snprintf(video_config.bitrate_str, sizeof(video_config.bitrate_str), "%d", bitrate);

        video_config_.push_back(video_config);
    }
}

void Client::onSetVideoConfig() {
    infra::WorkThreadPool::instance()->async([this] () {
        Json::Value video_config = Json::objectValue;
        Json::Value config = Json::arrayValue;
        for (int i = 0; i < video_config_.size(); i++) {
            VideoConfig &video_config = video_config_[i];
            Json::Value item = Json::objectValue;
            item["codec"] = video_config.encode_str_list[video_config.encode_type_index];

            int32_t width, height;
            (void)sscanf(video_config.resolution_str_list[video_config.resolution_index], "%dx%d", &width, &height);
            item["width"] = width;
            item["height"] = height;

            item["fps"] = atoi(video_config.fps_str);
            item["gop"] = atoi(video_config.gop_str);
            item["bitrate"] = atoi(video_config.bitrate_str);
            item["bitrate_type"] = video_config.bitrate_type_str_list[video_config.bitrate_type_index];
            item["image_quality"] = video_config.image_quality;
            item["bitrate_smooth"] = video_config.bitrate_smooth;
            
            config.append(item);
        }
        video_config["config"] = config;
        client_->setVideoConfig(video_config);

        Json::Value ret_config;
        client_->getVideoConfig(ret_config);
        infof("get_video_config:%s\n", ret_config.toStyledString());
        onGetVideoConfig(ret_config);
    });
}

void Client::onSetVideoConfig(int32_t stream) {
    VideoConfig &video_config = video_config_[stream];
    Json::Value config = Json::objectValue;
    config["sub_channel"] = stream;

    config["codec"] = video_config.encode_str_list[video_config.encode_type_index];

    int32_t width, height;
    (void)sscanf(video_config.resolution_str_list[video_config.resolution_index], "%dx%d", &width, &height);
    config["width"] = width;
    config["height"] = height;

    config["fps"] = atoi(video_config.fps_str);
    config["gop"] = atoi(video_config.gop_str);
    config["bitrate"] = atoi(video_config.bitrate_str);
    config["bitrate_type"] = video_config.bitrate_type_str_list[video_config.bitrate_type_index];
    config["image_quality"] = video_config.image_quality;
    config["bitrate_smooth"] = video_config.bitrate_smooth;

    client_->setVideoConfig(config);

    Json::Value ret_config;
    client_->getVideoConfig(ret_config);
    infof("get_video_config:%s\n", ret_config.toStyledString());
    onGetVideoConfig(ret_config);
}

void Client::interaction_tab_login() {
    if (ImGui::BeginTabItem("login")) {
        ImGui::Text("server ip:  ");
        ImGui::SameLine();
        ImGui::InputText("##server_ip", interaction_login_.server_ip, sizeof(interaction_login_.server_ip));
        ImGui::Text("server port:");
        ImGui::SameLine();
        ImGui::InputText("##server_port", interaction_login_.server_port, sizeof(interaction_login_.server_port), ImGuiInputTextFlags_CharsDecimal);
        ImGui::Text("username:   ");
        ImGui::SameLine();
        ImGui::InputText("##username", interaction_login_.username, sizeof(interaction_login_.username));
        ImGui::Text("password:   ");
        ImGui::SameLine();
        ImGui::InputText("##password", interaction_login_.password, sizeof(interaction_login_.password), ImGuiInputTextFlags_Password);
        
        ImGui::Text("            ");
        ImGui::SameLine();
        if (ImGui::Button("Login")) {
            login();
        }
        ImGui::SameLine();
        if (ImGui::Button("Logout")) {
            logout();
        }
        
        ImGui::EndTabItem();
    }
}

void Client::interaction_tab_video() {

    if (ImGui::BeginTabItem("video")) {
        ImGui::Text(u8"��ʽ: ");
        ImGui::SameLine();
        if (ImGui::Combo("##video_format", &video_input_format_index_, &video_input_format_str_list_[0], video_input_format_str_list_.size())) {
            onSetVideoFormat(video_input_format_index_);
        }
        ImGui::Separator();
        ImGui::SeparatorText("video");
        int32_t stream_count = video_config_.size();
        if (stream_count && ImGui::BeginTable("split", stream_count + 1)) {
            ImGui::TableNextColumn(); ImGui::Text("stream type: ");
            for (int i = 0; i < stream_count; i++) {
                std::string tmp = "  stream " + std::to_string(i + 1);
                ImGui::TableNextColumn(); ImGui::Text(tmp.data());
            }

            ImGui::TableNextColumn(); ImGui::Text("encode type:");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_encode" + std::to_string(i);
                ImGui::Combo(name_tmp.data(), &video_config_[i].encode_type_index, &video_config_[i].encode_str_list[0], video_config_[i].encode_str_list.size());
            }

            ImGui::TableNextColumn(); ImGui::Text("resolution:");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_resolution" + std::to_string(i);
                ImGui::Combo(name_tmp.data(), &video_config_[i].resolution_index, &video_config_[i].resolution_str_list[0], video_config_[i].resolution_str_list.size());
            }

            ImGui::TableNextColumn(); ImGui::Text("fps:");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_fps" + std::to_string(i);
                ImGui::InputText(name_tmp.data(), video_config_[i].fps_str, sizeof(video_config_[i].fps_str), ImGuiInputTextFlags_CharsDecimal);
            }

            ImGui::TableNextColumn(); ImGui::Text("gop:");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_gop" + std::to_string(i);
                ImGui::InputText(name_tmp.data(), video_config_[i].gop_str, sizeof(video_config_[i].gop_str), ImGuiInputTextFlags_CharsDecimal);
            }

            ImGui::TableNextColumn(); ImGui::Text("bitrate type: ");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_bitrate_type" + std::to_string(i);
                ImGui::Combo(name_tmp.data(), &video_config_[i].bitrate_type_index, &video_config_[i].bitrate_type_str_list[0], video_config_[i].bitrate_type_str_list.size());
            }

            ImGui::TableNextColumn(); ImGui::Text("bitrate(kbps): ");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##video_bitrate" + std::to_string(i);
                ImGui::InputText(name_tmp.data(), video_config_[i].bitrate_str, sizeof(video_config_[i].bitrate_str), ImGuiInputTextFlags_CharsDecimal);
            }

            uint8_t image_quality_min = 0;
            uint8_t image_quality_max = 100;
            static uint8_t image_quality = 50;
            ImGui::TableNextColumn(); ImGui::Text("image quality: ");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##image_quality" + std::to_string(i);
                ImGui::SliderScalar(name_tmp.data(), ImGuiDataType_U8, &video_config_[i].image_quality, &image_quality_min, &image_quality_max, "%d");
            }

            static uint8_t bitrate_smooth = 50;
            ImGui::TableNextColumn(); ImGui::Text("bitrate smooth: ");
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string name_tmp = "##bitrate_smooth" + std::to_string(i);
                ImGui::SliderScalar(name_tmp.data(), ImGuiDataType_U8, &video_config_[i].bitrate_smooth, &image_quality_min, &image_quality_max, "%d");
            }

            ImGui::TableNextColumn();
            for (int i = 0; i < stream_count; i++) {
                ImGui::TableNextColumn();
                std::string button_name = " set sub_channel " + std::to_string(i);
                if (ImGui::Button(button_name.data())) {
                    onSetVideoConfig();
                }
            }

            ImGui::EndTable();
        }
        ImGui::Text("This is the Avocado tab!blah blah blah blah blah");
        ImGui::EndTabItem();
    }
}

void Client::interaction(void* window) {
    ImGuiIO& io = ImGui::GetIO();

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    ImGui::Begin("config");
    if (ImGui::BeginTabBar("root_tabbar", tab_bar_flags)) {
        interaction_tab_login();
        interaction_tab_video();

        if (ImGui::BeginTabItem("audio")) {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("image")) {
            ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    if (show_demo_window_) {
        ImGui::ShowDemoWindow(&show_demo_window_);
    }
}

}
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
    if (!event_client_->connect(server_ip_.data(), server_port_)) {
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

    std::string video_format;
    client_->getVideoFormat(video_format);
    if (video_format == "pal") {
        video_input_format_index_ = 0;
    } else if (video_format == "ntsc") {
        video_input_format_index_ = 1;
    }

    Json::Value video_config;
    client_->getVideoConfig(video_config);
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
        if (ImGui::BeginTable("split", 3)) {
            ImGui::TableNextColumn(); ImGui::Text("stream type: ");
            ImGui::TableNextColumn(); ImGui::Text("    main");
            ImGui::TableNextColumn(); ImGui::Text("    sub");

            ImGui::TableNextColumn(); ImGui::Text("encode type:");
            ImGui::TableNextColumn(); ImGui::Combo("##main_encode", &main_video_encode_type_index_, &video_codec_type_[0], video_codec_type_.size());
            ImGui::TableNextColumn(); ImGui::Combo("##sub_encode", &sub_video_encode_type_index_, &video_codec_type_[0], video_codec_type_.size());

            ImGui::TableNextColumn(); ImGui::Text("resolution:");
            ImGui::TableNextColumn(); ImGui::Combo("##main_resolution", &main_resolution_index_, &resolution_[0], resolution_.size());
            ImGui::TableNextColumn(); ImGui::Combo("##sub_resolution", &sub_resolution_index_, &resolution_[0], resolution_.size());

            static char main_fps[32] = { "25" };
            static char sub_fps[32] = { "25" };
            ImGui::TableNextColumn(); ImGui::Text("fps:");
            ImGui::TableNextColumn(); ImGui::InputText("##main_fps", main_fps, sizeof(main_fps), ImGuiInputTextFlags_CharsDecimal);
            ImGui::TableNextColumn(); ImGui::InputText("##sub_fps", sub_fps, sizeof(sub_fps), ImGuiInputTextFlags_CharsDecimal);

            static char main_gop[32] = { "50" };
            static char sub_gop[32] = { "50" };
            ImGui::TableNextColumn(); ImGui::Text("GOP:");
            ImGui::TableNextColumn(); ImGui::InputText("##main_gop", main_gop, sizeof(main_gop), ImGuiInputTextFlags_CharsDecimal);
            ImGui::TableNextColumn(); ImGui::InputText("##sub_gop", sub_gop, sizeof(sub_gop), ImGuiInputTextFlags_CharsDecimal);

            ImGui::TableNextColumn(); ImGui::Text("bitrate type: ");
            ImGui::TableNextColumn(); ImGui::Combo("##main_bitrate_type", &main_bitrate_type_, &bitrate_type_[0], bitrate_type_.size());
            ImGui::TableNextColumn(); ImGui::Combo("##sub_bitrate_type", &sub_bitrate_type_, &bitrate_type_[0], bitrate_type_.size());

            static char main_bitrate[32] = { "2048"};
            static char sub_bitrate[32] = { "450"};
            ImGui::TableNextColumn(); ImGui::Text("bitrate(kbps): ");
            ImGui::TableNextColumn(); ImGui::InputText("##main_decimal", main_bitrate, sizeof(main_bitrate), ImGuiInputTextFlags_CharsDecimal);
            ImGui::TableNextColumn(); ImGui::InputText("##sub_decimal", sub_bitrate, sizeof(sub_bitrate), ImGuiInputTextFlags_CharsDecimal);

            uint8_t image_quality_min = 0;
            uint8_t image_quality_max = 100;
            static uint8_t image_quality = 50;
            ImGui::TableNextColumn(); ImGui::Text("image quality: ");
            ImGui::TableNextColumn(); ImGui::SliderScalar("##main_image_quality", ImGuiDataType_U8, &image_quality, &image_quality_min, &image_quality_max, "%d");
            ImGui::TableNextColumn(); ImGui::SliderScalar("##sub_image_quality", ImGuiDataType_U8, &image_quality, &image_quality_min, &image_quality_max, "%d");

            static uint8_t bitrate_smooth = 50;
            ImGui::TableNextColumn(); ImGui::Text("bitrate smooth: ");
            ImGui::TableNextColumn(); ImGui::SliderScalar("##main_bitrate_smooth", ImGuiDataType_U8, &bitrate_smooth, &image_quality_min, &image_quality_max, "%d");
            ImGui::TableNextColumn(); ImGui::SliderScalar("##sub_bitrate_smooth", ImGuiDataType_U8, &bitrate_smooth, &image_quality_min, &image_quality_max, "%d");

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
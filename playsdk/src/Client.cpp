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
    resolution_.push_back("2560x1440");
    resolution_.push_back("1920x1080");
    resolution_.push_back("1280x720");
    resolution_.push_back("640x480");

    bitrate_type_.push_back("VBR");
    bitrate_type_.push_back("CBR");

    video_codec_type_.push_back("H.264");
    video_codec_type_.push_back("H.265");
}

void Client::interaction_tab_video() {
    if (ImGui::BeginTabItem("video")) {
        if (ImGui::BeginTable("split", 3)) {
            ImGui::TableNextColumn(); ImGui::Text("stream type: ");
            ImGui::TableNextColumn(); ImGui::Text("    main");
            ImGui::TableNextColumn(); ImGui::Text("    sub");

            ImGui::TableNextColumn(); ImGui::Text("resolution:");
            ImGui::TableNextColumn(); ImGui::Combo("##main_resolution", &main_resolution_index_, &resolution_[0], resolution_.size());
            ImGui::TableNextColumn(); ImGui::Combo("##sub_resolution", &sub_resolution_index_, &resolution_[0], resolution_.size());

            ImGui::TableNextColumn(); ImGui::Text("encode type:");
            ImGui::TableNextColumn(); ImGui::Combo("##main_encode", &main_video_encode_type_index_, &video_codec_type_[0], video_codec_type_.size());
            ImGui::TableNextColumn(); ImGui::Combo("##sub_encode", &sub_video_encode_type_index_, &video_codec_type_[0], video_codec_type_.size());

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
        interaction_tab_video();

        if (ImGui::BeginTabItem("audio")) {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Cucumber")) {
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
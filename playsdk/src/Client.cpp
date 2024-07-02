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

void Client::interaction_tab_video() {
    if (ImGui::BeginTabItem("video")) {
        if (ImGui::BeginTable("split", 3)) {
            ImGui::TableNextColumn(); ImGui::Text("stream type: ");
            ImGui::TableNextColumn(); ImGui::Text("main");
            ImGui::TableNextColumn(); ImGui::Text("sub");

            ImGui::TableNextColumn(); ImGui::Text("resolution:");
            ImGui::TableNextColumn();
            const char* items0[] = { "1920x1080", "640x480" };
            static int item_current0 = 0;
            ImGui::Combo("", &item_current0, items0, IM_ARRAYSIZE(items0));

            ImGui::TableNextColumn();
            const char* items1[] = { "1920x1080", "640x480" };
            static int item_current1 = 0;
            ImGui::Combo("0", &item_current0, items1, IM_ARRAYSIZE(items1));

            ImGui::TableNextColumn(); ImGui::Text("bitrate type:");
            ImGui::TableNextColumn();
            const char* bitrate_type[] = { "VBR", "CBR" };
            ImGui::Combo("1", &item_current0, bitrate_type, IM_ARRAYSIZE(bitrate_type));
            ImGui::TableNextColumn();
            ImGui::Combo("2", &item_current0, bitrate_type, IM_ARRAYSIZE(bitrate_type));

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
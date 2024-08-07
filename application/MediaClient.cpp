#include "MediaClient.h"
#include "infra/include/Logger.h"

MediaClient::MediaClient() {
    client_ = IPrivClient::create();
    event_client_ = IPrivClient::create();
    playsdk_ = playsdk::IPlaysdk::create();
    server_ip_ = "172.16.9.100";
    server_port_ = 7000;
}

bool MediaClient::init() {
    if (!playsdk_->init(playsdk::PlayModeLive)) {
        errorf("playsdk init failed\n");
        return false;
    }
    playsdk_->start();
    return true;

    bool ret = client_->connect(server_ip_.data(), server_port_);
    if (!ret) {
        return false;
    }
    client_->startPreview(0, 0, IPrivClient::OnFrameProc(&MediaClient::onMediaFrame, this));

    /*if (!playsdk_->init(playsdk::PlayModeFile)) {
        errorf("playsdk init failed\n");
        return false;
    }
    playsdk_->setMediaFileName("F:\\mp4\\HD1080P.X264.AAC.Mandarin.CHS.BDYS.mp4");
    playsdk_->start();
    playsdk_->setSpeed(2.9f);
    */
    return true;
}

void MediaClient::onMediaFrame(MediaFrameType type, MediaFrame& frame) {
    //tracef("onmedia size:%d\n", frame.size());
    playsdk_->inputMediaFrame(frame);
}

bool MediaClient::subscribeEvent() {
    if (!event_client_->connect(server_ip_.data(), server_port_)) {
        return false;
    }

    event_client_->subscribeEvent("detect_target", [this](Json::Value& data) {
        //tracef("onEvent:%s\n", data.toStyledString().data());
        playsdk_->setTrackingBox(data);
    });

    return true;
}
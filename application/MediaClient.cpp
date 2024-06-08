#include "MediaClient.h"
#include "infra/include/Logger.h"

MediaClient::MediaClient() {
    client_ = IPrivClient::create();
    playsdk_ = playsdk::IPlaysdk::create();
}

bool MediaClient::init() {
    //bool ret = client_->connect("127.0.0.1", 7000);
    //if (!ret) {
    //    return false;
    //}
    //client_->startPreview(0, 0, IPrivClient::OnFrameProc(&MediaClient::onMediaFrame, this));

    if (!playsdk_->init(playsdk::PlayModeFile)) {
        errorf("playsdk init failed\n");
        return false;
    }
    playsdk_->setMediaFileName("F:\\mp4\\HD1080P.X264.AAC.Mandarin.CHS.BDYS.mp4");
    playsdk_->start();

    return true;
}

void MediaClient::onMediaFrame(MediaFrameType type, MediaFrame& frame) {
    tracef("onmedia\n");
}


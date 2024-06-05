#include "MediaClient.h"
#include "infra/include/Logger.h"

MediaClient::MediaClient() {
    client_ = IPrivClient::create();
    decoder_ = std::make_shared<playsdk::Decoder>();
}

bool MediaClient::init() {
    //bool ret = client_->connect("127.0.0.1", 7000);
    //if (!ret) {
    //    return false;
    //}
    //client_->startPreview(0, 0, IPrivClient::OnFrameProc(&MediaClient::onMediaFrame, this));

    decoder_->init();

    return true;
}

void MediaClient::onMediaFrame(MediaFrameType type, MediaFrame& frame) {
    tracef("onmedia\n");
}


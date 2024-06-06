/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  playsdk.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:04:14
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "playsdk.h"
#include "infra/include/Logger.h"

namespace playsdk {

std::shared_ptr<IPlaysdk> IPlaysdk::create() {
    return std::make_shared<Playsdk>();
}

Playsdk::Playsdk() {
    decoder_ = std::make_shared<Decoder>();
    render_ = std::make_shared<Render>();
}

Playsdk::~Playsdk() {
}

bool Playsdk::init() {
    render_->initial();
    return true;
}

bool Playsdk::inputMediaFrame(MediaFrame frame) {
    return true;
}


}
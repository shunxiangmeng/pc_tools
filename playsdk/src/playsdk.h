/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  playsdk.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:04:49
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <memory>
#include "playsdk/include/IPlaysdk.h"
#include "codec/decoder.h"
#include "render/render.h"

namespace playsdk {

class Playsdk : public IPlaysdk {
public:
    Playsdk();
    virtual ~Playsdk();

    virtual bool init() override;
    virtual bool inputMediaFrame(MediaFrame frame) override;

private:
    std::shared_ptr<Decoder> decoder_;
    std::shared_ptr<Render> render_;
};
    
}
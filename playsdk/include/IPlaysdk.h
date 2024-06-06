/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  playsdk.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:01:14
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <memory>
#include "common/mediaframe/MediaFrame.h"

namespace playsdk {

class IPlaysdk {
public:
    IPlaysdk() = default;

    virtual ~IPlaysdk() = default;

    static std::shared_ptr<IPlaysdk> create();

    virtual bool init() = 0;

    virtual bool inputMediaFrame(MediaFrame frame) = 0;
};

}
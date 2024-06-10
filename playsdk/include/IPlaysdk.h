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

typedef enum {
    PlayModeLive = 0,
    PlayModeFile
}PlayMode;

class IPlaysdk {
public:
    IPlaysdk() = default;

    virtual ~IPlaysdk() = default;

    static std::shared_ptr<IPlaysdk> create();

    virtual bool init(PlayMode playmode) = 0;

    virtual bool inputMediaFrame(MediaFrame frame) = 0;

    virtual bool setMediaFileName(const char* filename) = 0;

    virtual bool start() = 0;

    virtual bool setSpeed(float speed) = 0;
    
    virtual float speed()  = 0;
};

}
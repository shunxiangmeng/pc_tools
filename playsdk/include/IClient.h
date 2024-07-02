/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  IClient.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-02 09:57:17
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <memory>
namespace playsdk {

class IClient : public std::enable_shared_from_this<IClient> {
public:
    IClient() = default;
    virtual ~IClient() = default;
    static std::shared_ptr<IClient> create();
    virtual bool init() = 0;
    virtual void interaction(void* window) = 0;
};

}
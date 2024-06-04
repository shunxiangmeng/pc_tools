/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Main.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-28 13:10:01
 * Description :  None
 * Note        : 
 ************************************************************************/
#include <chrono>
#include <thread>
#include "infra/include/Logger.h"
#include "infra/include/network/Network.h"
#include "infra/include/network/NetworkThreadPool.h"
#include "infra/include/thread/WorkThreadPool.h"
#include "configManager/include/IConfigManager.h"
#include "private/include/IPrivClient.h"
#include "OacClientTest.h"
#include "MediaClient.h"

int main(int argc, char* argv[]) {

    std::shared_ptr<infra::LogChannel> console_log = std::make_shared<infra::ConsoleLogChannel>();
    infra::Logger::instance().addLogChannel(console_log);

    std::string config_path = "";
    std::string default_config_path = "";
    IConfigManager::instance()->init(config_path.data(), default_config_path.data());

    infof("bronco start............\n");

    infra::network_init();
    infra::NetworkThreadPool::instance()->init(4);
    infra::WorkThreadPool::instance()->init(4);

    OacClientTest oac_client;
    //oac_client.init();



    MediaClient media_client;
    media_client.init();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

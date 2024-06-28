#include <memory>
#include "private/include/IPrivClient.h"
#include "playsdk/include/IPlaysdk.h"

class MediaClient {
public:
    MediaClient();
    bool init();
    bool subscribeEvent();

private:
    void onMediaFrame(MediaFrameType type, MediaFrame& frame);
private:
    std::string server_ip_;
    std::shared_ptr<IPrivClient> client_;
    std::shared_ptr<playsdk::IPlaysdk> playsdk_;
    std::shared_ptr<IPrivClient> event_client_;
    
};
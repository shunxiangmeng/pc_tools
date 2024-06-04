#include <memory>
#include "private/include/IPrivClient.h"

class MediaClient {
public:
    MediaClient();
    bool init();

private:
    void onMediaFrame(MediaFrameType type, MediaFrame& frame);
private:
    std::shared_ptr<IPrivClient> client_;
};
#include <memory>
#include "private/include/IPrivClient.h"
#include "playsdk/src/codec/decoder.h"

class MediaClient {
public:
    MediaClient();
    bool init();

private:
    void onMediaFrame(MediaFrameType type, MediaFrame& frame);
private:
    std::shared_ptr<IPrivClient> client_;
    std::shared_ptr<playsdk::Decoder> decoder_;
    
};
/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  render.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:37:35
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <future>
#include <queue>
#include "glad/glad.h"
#include "glfw/include/glfw3.h"
#include "infra/include/thread/Thread.h"
#include "jsoncpp/include/json.h"
#include "../DecodedFrame.h"
#include "polygon.h"
#include "text.h"
#include "hal/Defines.h"
#include "playsdk/include/IClient.h"

namespace playsdk {

class Render : public infra::Thread {
public:
    Render(DecodedFrameList& video_decoded_frame_queue);
    ~Render();

    bool initial();
    void setSpeed(float speed);
    int64_t getCurrentPts() {
        return current_pts_;
    }
    void setAudioCurrentPts(int64_t pts);
    bool setTrackingBox(Json::Value data);
    void setClient(std::weak_ptr<IClient> client);
private:
    virtual void run() override;

    GLFWwindow* initWindowEnvironment();
    void processInput(GLFWwindow* window);

    bool initShader();
    bool initShaders();
    bool initGui(GLFWwindow* window);

    void renderVideoFrame(GLFWwindow* window);
    void readerTextInfo(GLFWwindow* window);
    void renderTrackingBox(GLFWwindow* window);
    void setCenterScale(GLFWwindow* window, int32_t video_w, int32_t video_h);
    void adaptiveRender(std::vector<std::vector<Position>>& polyons);
    void renderGui(GLFWwindow* window);

private:
    std::weak_ptr<IClient> client_;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<std::promise<bool>> init_promise_;
    DecodedFrameList& video_decoded_frame_queue_;
    int32_t window_width_;
    int32_t window_height_;

    GLuint VAO_ = 0;
    GLuint VBO_ = 0;
    GLuint EBO_ = 0;
    GLuint gl_textureid_[3];
    float center_scale_x_ = 1.0f;
    float center_scale_y_ = 1.0f;

    Polygon polyon_;
    Text text_;

    int64_t current_pts_ = 0;
    int64_t audio_current_pts_ = 0;
    float speed_ = 1.0f;

    std::mutex tracking_box_list_mutex_;
    std::queue<std::shared_ptr<CurrentDetectResult>> tracking_box_list_;
};

typedef struct {
    std::mutex mutex;
    float whell_scroll_x;
    float whell_scroll_y;
} WindowEvent;

}
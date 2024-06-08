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
#include "glad/glad.h"
#include "glfw/include/glfw3.h"
#include "infra/include/thread/Thread.h"
#include "../DecodedFrame.h"
#include "polygon.h"

namespace playsdk {

class Render : public infra::Thread {
public:
    Render(DecodedFrameList& video_decoded_frame_queue);
    ~Render();

    bool initial();

private:
    virtual void run() override;

    GLFWwindow* initWindowEnvironment();
    bool initShader();
    bool initShaders();

    void renderVideoFrame(GLFWwindow* window);
    void setCenterScale(GLFWwindow* window, int32_t video_w, int32_t video_h);

private:
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
};

}
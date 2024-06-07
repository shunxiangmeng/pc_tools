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

    bool initShaders();
    bool checkShader(GLint shader);
    bool CheckProgram(GLuint prog);

    void renderVideoFrame();

private:
    std::shared_ptr<std::promise<bool>> init_promise_;
    DecodedFrameList& video_decoded_frame_queue_;
    int32_t window_width_;
    int32_t window_height_;

    GLuint gl_program_{0};
    GLuint gl_vao_{0};
    GLuint gl_textureid_[3];

    Polygon polyon_;
};

}
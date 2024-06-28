/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  gui.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-28 11:17:20
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "guiBase.h"

namespace playsdk {
class Gui {
public:
    Gui(std::string name);
    ~Gui();
    bool initialize(int32_t width, int32_t height);
    bool resize(int32_t width, int32_t height);
    void render(const glm::mat4& p, const glm::mat4& v);
    void setModel(const glm::mat4& m);
    void getWidthHeight(float& width, float& height);
    void active();
    void begin(int32_t x, int32_t y, int32_t width, int32_t height);
    void end();
    void triggerEvent(bool down);
    void updateMousePosition(float x, float y);

private:
    bool initShader();
private:
    static Shader shader_;
    std::string mName;

    GLuint gl_frame_buffer_;
    GLuint texture_colorbuffer_;
    GLuint mVAO;
    GLuint mVBO;

    int32_t width_;
    int32_t height_;

    glm::mat4 model_;
    glm::vec3 mIntersectionPoint;
};
}
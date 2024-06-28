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
    void render(const glm::mat4& p, const glm::mat4& v);
    void setModel(const glm::mat4& m);
    void getWidthHeight(float& width, float& height);
    bool isIntersectWithLine(const glm::vec3& linePoint, const glm::vec3& lineDirection);
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

    GLuint mFramebuffer;
    GLuint mTextureColorbuffer;
    GLuint mVAO;
    GLuint mVBO;

    int32_t mWidth;
    int32_t mHeight;

    glm::mat4 mModel;
    glm::vec3 mIntersectionPoint;
};
}
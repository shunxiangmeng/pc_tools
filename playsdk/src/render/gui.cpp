/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  gui.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-28 11:17:04
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "gui.h"
#include "utils.h"
#include "glm/geometric.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace playsdk {
Shader Gui::shader_;
Gui::Gui(std::string name): mName(name), gl_frame_buffer_(0), texture_colorbuffer_(0), mVAO(0), mVBO(0) {
    model_ = glm::mat4(1.0f);
}

Gui::~Gui() {
}

bool Gui::initShader() {
    static bool init = false;
    if (init) {
        return true;
    }
    else {
        const GLchar* vertex_shader_glsl = R"_(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            layout (location = 1) in vec2 aTexCoords;
            out vec2 TexCoords;
            out vec3 FragPos;
            uniform mat4 projection;
            uniform mat4 view;
            uniform mat4 model;
            void main()
            {
                FragPos = vec3(model * vec4(aPos, 1.0));
                TexCoords = aTexCoords;
                gl_Position = projection * view * vec4(FragPos, 1.0);
            }
        )_";

        const GLchar* fragment_shader_glsl = R"_(
            #version 330 core
            out vec4 FragColor;
            in vec2 TexCoords;
            uniform sampler2D screenTexture;
            in vec3 FragPos;
            uniform vec3 intersectionPoint;
            void main()
            {
                float distance = (FragPos.x-intersectionPoint.x) * (FragPos.x-intersectionPoint.x) + (FragPos.y-intersectionPoint.y) * (FragPos.y-intersectionPoint.y) + (FragPos.z-intersectionPoint.z) * (FragPos.z-intersectionPoint.z);
                if (distance < 0.0001) {
                    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
                } else {
                    FragColor = texture(screenTexture, TexCoords);
                }
            }
        )_";

        if (shader_.loadShader(vertex_shader_glsl, fragment_shader_glsl) == false) {
            return false;
        }
        init = true;
    }
    return true;
}

bool Gui::initialize(int32_t width, int32_t height) {
    width_ = width;
    height_ = height;

    GuiBase::instance().initialize();

    if (!initShader()) {
        return false;
    }
    if (gl_frame_buffer_) {
        return true;
    }

    GL_CALL(glGenFramebuffers(1, &gl_frame_buffer_));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer_));

    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glGenTextures(1, &texture_colorbuffer_));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer_, 0));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        errorf("Framebuffer is not complete!");
        return false;
    }
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    const float vertices[] = {
        // positions         // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    GL_CALL(glGenVertexArrays(1, &mVAO));
    GL_CALL(glGenBuffers(1, &mVBO));
    GL_CALL(glBindVertexArray(mVAO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
    return true;
}

bool Gui::resize(int32_t width, int32_t height) {
    if (width == width_ && height == height_) {
        return true;
    }
    width_ = width;
    height_ = height;

    if (texture_colorbuffer_) {
        GL_CALL(glDeleteTextures(1, &texture_colorbuffer_));
        texture_colorbuffer_ = 0;
    }
    if (gl_frame_buffer_ == 0) {
        GL_CALL(glGenFramebuffers(1, &gl_frame_buffer_));
    }
    
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer_));
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glGenTextures(1, &texture_colorbuffer_));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer_, 0));
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        errorf("Framebuffer is not complete!\n");
        return false;
    }
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Gui::render(const glm::mat4& p, const glm::mat4& v) {

    GLenum last_framebuffer = 0; 
    GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&last_framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, gl_frame_buffer_));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_colorbuffer_, 0));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    active();
    GuiBase::instance().render();

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, last_framebuffer));

    shader_.use(); 
    shader_.setUniformMat4("projection", p);
    shader_.setUniformMat4("view", v);
    shader_.setUniformMat4("model", model_);
    shader_.setUniformVec3("intersectionPoint", mIntersectionPoint);

    GL_CALL(glDisable(GL_CULL_FACE));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glBindVertexArray(mVAO));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_colorbuffer_));
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void Gui::setModel(const glm::mat4& m) {
    model_ = m;
}

void Gui::updateMousePosition(float x, float y) {
    //active();
    auto& io = ImGui::GetIO();
    io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    io.AddMousePosEvent(x, y);
}

void Gui::triggerEvent(bool down) {
    //active();
    auto& io = ImGui::GetIO();
    io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    io.AddMouseButtonEvent(0/*left button*/, down);
}

void Gui::getWidthHeight(float& width, float& height) {
    width = float(width_);
    height = float(height_);
}

void Gui::active() {
    auto& io = ImGui::GetIO();
    io.DisplaySize.x = float(width_);
    io.DisplaySize.y = float(height_);
    io.DeltaTime = 1.0f / 60.0f;
}

void Gui::begin(int32_t x, int32_t y, int32_t width, int32_t height) {
    //active();
    auto& io = ImGui::GetIO();
    io.DisplaySize.x = float(width);
    io.DisplaySize.y = float(height);
    io.DeltaTime = 1.0f / 60.0f;

    ImGui::NewFrame();
    ImGui::Begin(mName.c_str());
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::SetWindowPos(ImVec2(10, 10));
    ImGui::SetWindowFocus();
}

void Gui::end() {
    ImGui::End();
    ImGui::Render();
}
}
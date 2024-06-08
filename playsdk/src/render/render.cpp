/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  render.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:37:31
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "render.h"
#include "shaders.h"
#include "infra/include/Logger.h"
#include "infra/include/Timestamp.h"

namespace playsdk {

Render::Render(DecodedFrameList& video_decoded_frame_queue) : video_decoded_frame_queue_(video_decoded_frame_queue) {
    window_width_ = 0;
    window_height_ = 0;
}

Render::~Render() {
}

bool Render::initial() {
    init_promise_ = std::make_shared<std::promise<bool>>();
    std::future<bool> future = init_promise_->get_future();
    infra::Thread::start();
    auto wait_result = future.wait_for(std::chrono::milliseconds(500));
    if (wait_result == std::future_status::timeout) {
        errorf("init timeout\n");
        return false;
    }
    return future.get();
}

bool Render::initShader() {
    if (shader_) {
        return true;
    }
    shader_ = std::make_shared<Shader>();

    const GLchar* vertex_shader_glsl = R"_(
        #version 330 core
        precision mediump float;
        attribute vec3 position;
        attribute vec2 coordniate;
        uniform float center_sacle_x;
        uniform float center_sacle_y;
        varying vec2 textureOut;
        void main(void)
        {
            gl_Position = vec4(position.x * center_sacle_x, position.y * center_sacle_y, position.z, 1.0);
            textureOut = coordniate;
        }
    )_";

    const GLchar* fragment_shader_glsl = R"_(
        #version 330 core
        varying vec2 textureOut;
        uniform sampler2D tex_y;
        uniform sampler2D tex_u;
        uniform sampler2D tex_v;
        void main(void)
        {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(tex_y, textureOut).r;
            yuv.g = texture2D(tex_u, textureOut).r - 0.5;
            yuv.b = texture2D(tex_v, textureOut).r - 0.5;
            rgb = mat3( 1.0,     1.0,       1.0,
                        0.0,     -0.21482,  2.12798,
                        1.28033, -0.38059,  0.0) * yuv;
            gl_FragColor = vec4(rgb, 1);
        }
    )_";

    if (!shader_->loadShader(vertex_shader_glsl, fragment_shader_glsl)) {
        shader_.reset();
        return false;
    }
    return true;
}

static const GLfloat s_vertices_coord[] = {
    // positions        // textureCoords
    -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,  // left top  
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f,  // right top 
     1.0f, -1.0f, 0.0f,   1.0f, 1.0f,  // right bottom
    -1.0f, -1.0f, 0.0f,   0.0f, 1.0f   // left bottom
};

static const unsigned int s_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

bool Render::initShaders() {
    if (!initShader()) {
        return false;
    }

    shader_->use();
    GLuint position = shader_->getAttribLocation("position");
    GLuint coordniate = shader_->getAttribLocation("coordniate");
    GLuint texturey = shader_->getUniformLocation("tex_y");
    GLuint textureu = shader_->getUniformLocation("tex_u");
    GLuint texturev = shader_->getUniformLocation("tex_v");

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(coordniate);

    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices_coord), s_vertices_coord, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indices), s_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(coordniate, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glUniform1i(texturey, 0);
    glUniform1i(textureu, 1);
    glUniform1i(texturev, 2);

    //texture
    glGenTextures(3, gl_textureid_);
    glBindTexture(GL_TEXTURE_2D, gl_textureid_[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, gl_textureid_[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, gl_textureid_[2]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    return true;
}

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLFWwindow* Render::initWindowEnvironment() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "playsdk", NULL, NULL);
    if (window == nullptr) {
        errorf("Failed to create GLFW window\n");
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        errorf("Failed to initialize GLAD\n");
        glfwTerminate();
        return nullptr;
    }
    return window;
}

void Render::setCenterScale(GLFWwindow* window, int32_t video_width, int32_t video_height) {
    int32_t window_width = 0;
    int32_t window_height = 0;
    //glfwGetFramebufferSize(window, &window_width, &window_height);
    glfwGetWindowSize(window, &window_width, &window_height);
    if (window_width == window_width_ && window_height == window_height_) {
        return;
    }
    window_width_ = window_width;
    window_height_ = window_height;

    float video_aspect_ratio = 1.0 * video_width / video_height;
    float window_aspect_ration = 1.0 * window_width / window_height;
    if (window_aspect_ration > video_aspect_ratio) {
        // adjust x
        float window_width_need = 1.0 * video_width / video_height * window_height;
        center_scale_x_ = window_width_need / window_width;
        center_scale_y_ = 1.0f;
    } else {
        // adjust y
        float window_height_need = 1.0 * video_height / video_width * window_width;
        center_scale_y_ = window_height_need / window_height;
        center_scale_x_ = 1.0f;
    }
    shader_->setUniformFloat("center_sacle_y", center_scale_y_);
    shader_->setUniformFloat("center_sacle_x", center_scale_x_);
}

void Render::renderVideoFrame(GLFWwindow* window) {
    if (video_decoded_frame_queue_.size()) {
        DecodedFrame frame = video_decoded_frame_queue_.front();
        int64_t now = infra::getCurrentTimeMs();
        //debugf("redner pts:%lld, now:%lld\n", frame.frame_->pkt_pts, now);
        if (frame.frame_->pkt_pts <= now) {
            if (video_decoded_frame_queue_.size() > 1) {
                video_decoded_frame_queue_.pop();
            }
        }

        shader_->use();
        glBindVertexArray(VAO_);

        int32_t width = frame.frame_->width;
        int32_t height = frame.frame_->height;
        setCenterScale(window, width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.frame_->data[2]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Render::run() {
    warnf("render thread start\n");
    bool init_reesult = false;
    GLFWwindow* window = nullptr;
    do {
        window = initWindowEnvironment();
        if (window == nullptr) {
            break;
        }
        if (!initShaders()) {
            glfwTerminate();
            break;
        }
        if (!polyon_.initialize()) {
            glfwTerminate();
            break;
        }
        init_reesult = true;
    } while (0);
    
    init_promise_->set_value(std::move(init_reesult));

    std::vector<std::vector<Position>> polyons;
    std::vector<Position> polyon;
    polyon.push_back({ 0.1f, 0.0f, 0.0f});
    polyon.push_back({ 1.0f, 0.0f, 0.0f });
    polyon.push_back({ 0.9f, 0.9f, 0.0f });
    polyon.push_back({ 0.0f, 1.0f, 0.0f });

    polyons.push_back(polyon);

    polyon_.setPointLines(polyons);

    while (!glfwWindowShouldClose(window) && running()) {
        // input
        processInput(window);
        glfwPollEvents();

        //render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderVideoFrame(window);

        polyon_.render();

        glfwSwapBuffers(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    glfwTerminate();
    warnf("render thread exit\n");
}

}
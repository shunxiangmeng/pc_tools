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

namespace playsdk {

Render::Render() {
    window_width_ = 800;
    window_height_ = 600;
}

Render::~Render() {
}

bool Render::initial() {
    return infra::Thread::start();
}

static const GLfloat s_vertices_coord[] = {
    // positions        // textureCoords
    0.5f,   0.5f, 0.0f,   1.0f, 1.0f,  // top right  
    1.0f,  -1.0f, 0.0f,   1.0f, 0.0f,  // bottom right 
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // bottom left
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f   // top left
};

static const unsigned int s_indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

bool Render::initShaders() {
    GLint vertex_compiled, fragment_compiled;

    GLint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    GLint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shader_vertex, 1, &g_vertex_shader, NULL);
    glCompileShader(shader_vertex);
    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &vertex_compiled);
    checkShader(shader_vertex);

    glShaderSource(shader_fragment, 1, &g_fragment_shader, NULL);
    glCompileShader(shader_fragment);
    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &fragment_compiled);
    checkShader(shader_fragment);

    gl_program_ = glCreateProgram();
    glAttachShader(gl_program_, shader_vertex);
    glAttachShader(gl_program_, shader_fragment);
    glLinkProgram(gl_program_);
    CheckProgram(gl_program_);
    glUseProgram(gl_program_);

    glDeleteShader(shader_vertex);
    glDeleteShader(shader_fragment);

    GLuint position = (GLuint)glGetAttribLocation(gl_program_, "position");
    GLuint coordnate = (GLuint)glGetAttribLocation(gl_program_, "coordnate");
    GLuint texturey = (GLuint)glGetUniformLocation(gl_program_, "tex_y");
    GLuint textureu = (GLuint)glGetUniformLocation(gl_program_, "tex_u");
    GLuint texturev = (GLuint)glGetUniformLocation(gl_program_, "tex_v");

    glGenVertexArrays(1, &gl_vao_);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(gl_vao_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glEnableVertexAttribArray(position);
    glEnableVertexAttribArray(coordnate);

    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices_coord), s_vertices_coord, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indices), s_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(coordnate, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

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

bool Render::checkShader(GLint shader) {
    GLint r = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetShaderInfoLog(shader, sizeof(msg), &length, msg);
        errorf("Compile shader failed: %s", msg);
        return false;
    }
    return true;
}

bool Render::CheckProgram(GLuint prog) {
    GLint r = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetProgramInfoLog(prog, sizeof(msg), &length, msg);
        errorf("Link program failed: %s", msg);
        return false;
    }
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
    GLFWwindow* window = glfwCreateWindow(window_width_, window_height_, "playsdk", NULL, NULL);
    if (window == nullptr) {
        errorf("Failed to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        errorf("Failed to initialize GLAD\n");
        return nullptr;
    }
    return window;
}

void Render::run() {
    warnf("render thread start\n");
    GLFWwindow* window = initWindowEnvironment();
    initShaders();

    typedef struct {
        int32_t width;
        int32_t height;
        char* data;
    }Frame;

    Frame frame;
    frame.width = 1920;
    frame.height = 1080;
    frame.data = (char*)malloc(frame.width * frame.height * 3 / 2);

    int32_t width = frame.width;
    int32_t height = frame.height;

    while (!glfwWindowShouldClose(window) && running()) {

        // input
        processInput(window);

        //render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(gl_program_);
        glBindVertexArray(gl_vao_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame.width, frame.height, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data + (width * height));

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gl_textureid_[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, frame.data + (width * height * 5 / 4));

        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
        glfwPollEvents();
        glfwSwapBuffers(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    glfwTerminate();
    warnf("render thread exit\n");
}

}
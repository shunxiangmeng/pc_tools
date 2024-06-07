/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  polygon.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-07 14:44:03
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "shaders.h"
#include <memory>
#include <vector>
#include "glfw/include/glfw3.h"
#include "utils.h"

namespace playsdk {

class Polygon {
public:
    Polygon();
    ~Polygon();

    bool initialize();
    void setPointLines(std::vector<std::vector<Position>> &polyons);
    bool render();
private:
    bool initShader();

private:
    std::shared_ptr<Shader> shader_;
    GLuint VAO_ = 0;
    GLuint VBO_ = 0;
    GLuint EBO_ = 0;
    std::vector<std::vector<Position>> polyons_;
    bool update_data_ = false;
};


}
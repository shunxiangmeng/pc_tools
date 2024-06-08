/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  polygon.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-07 14:44:50
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "polygon.h"

namespace playsdk {

Polygon::Polygon() {
}

Polygon::~Polygon() {
}

bool Polygon::initShader() {
    if (shader_) {
        return true;
    }
    shader_ = std::make_shared<Shader>();

    const GLchar* vertex_shader_glsl = R"_(
        #version 330 core
        attribute vec3 position;
        void main()
        {
            gl_Position = vec4(position, 1.0);
        }
    )_";

    const GLchar* fragment_shader_glsl = R"_(
        #version 330 core
        precision mediump float;
        uniform vec3 color;
        void main()
        {
            gl_FragColor = vec4(color, 1.0);
        }
    )_";

    if (!shader_->loadShader(vertex_shader_glsl, fragment_shader_glsl)) {
        shader_.reset();
        return false;
    }
    return true;
}

bool Polygon::initialize() {
    if (!initShader()) {
        return false;
    }

    GL_CALL(glGenVertexArrays(1, &VAO_));
    GL_CALL(glGenBuffers(1, &VBO_));
    GL_CALL(glGenBuffers(1, &EBO_));
    GL_CALL(glBindVertexArray(VAO_));
    //GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_));
    //GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_));
    //GL_CALL(glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW));
    //GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GL_CALL(glBindVertexArray(0));

    return true;
}

void Polygon::setPointLines(std::vector<std::vector<Position>>& polyons) {
    polyons_ = polyons;
    for (auto& polygon : polyons_) {
        for (auto& position : polygon) {
            if (position.x > 1.0f) {
                position.x = 1.0f;
            }
            if (position.x < 0.0f) {
                position.x = 0.0f;
            }
            if (position.y > 1.0f) {
                position.y = 1.0f;
            }
            if (position.y < 0.0f) {
                position.y = 0.0f;
            }

            position.y = 1 - position.y;

            position.x = position.x * 2 - 1;
            position.y = position.y * 2 - 1;
        }
    }
    update_data_ = true;
}

bool Polygon::render() {
    if (polyons_.size() == 0) {
        return true;
    }

    shader_->use();
    glLineWidth(2.0f);

    std::vector<GLuint> indices;
    if (1 /*update_data_*/) {
        update_data_ = false;
        GL_CALL(glBindVertexArray(VAO_));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_));
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_));

        shader_->setUniformVec3("color", 1.0, 0.0, 0.0);

        GLuint position = shader_->getAttribLocation("position");
        GL_CALL(glEnableVertexAttribArray(position));

        for (auto& pol : polyons_) {
            GL_CALL(glBufferData(GL_ARRAY_BUFFER, pol.size() * sizeof(Position), pol.data(), GL_DYNAMIC_DRAW));
            GL_CALL(glVertexAttribPointer(position, sizeof(Position) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Position), (const void*)0));
            
            for (unsigned int i = 0; i < pol.size(); i++) {
                indices.push_back(i);
            }
            GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW));
        }

        //GL_CALL(glBindVertexArray(VAO_));
        GL_CALL(glDrawElements(GL_LINE_LOOP, indices.size(), GL_UNSIGNED_INT, 0));

        GL_CALL(glBindVertexArray(GL_NONE));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL_NONE));
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE));
    }
    return true;
}

}
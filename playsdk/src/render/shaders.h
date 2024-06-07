/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  shaders.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:49:59
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <string>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace playsdk {
extern const char* g_vertex_shader;
extern const char* g_fragment_shader;

class Shader {
public:
    Shader();
    ~Shader();

    bool loadShader(const char* vertex_code, const char* fragment_code);

    void use() const;
    GLuint id() const;
    void setUniformBool(const std::string& name, bool value) const;
    void setUniformInt(const std::string& name, int value) const;
    void setUniformFloat(const std::string& name, float value) const;
    void setUniformVec2(const std::string& name, const glm::vec2& value) const;
    void setUniformVec2(const std::string& name, float x, float y) const;
    void setUniformVec3(const std::string& name, const glm::vec3& value) const;
    void setUniformVec3(const std::string& name, float x, float y, float z) const;
    void setUniformVec4(const std::string& name, const glm::vec4& value) const;
    void setUniformVec4(const std::string& name, float x, float y, float z, float w) const;
    void setUniformMat2(const std::string& name, const glm::mat2& mat) const;
    void setUniformMat3(const std::string& name, const glm::mat3& mat) const;
    void setUniformMat4(const std::string& name, const glm::mat4& mat) const;
    GLuint getAttribLocation(const std::string& name) const;
private:
    bool checkCompileErrors(GLuint shader, std::string type);

private:
    GLuint program_;
};


}
/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  shaders.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:50:16
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "shader.h"
#include "utils.h"

namespace playsdk {

Shader::Shader() : program_(0) {
}

Shader::~Shader() {
    if (program_) {
        glDeleteProgram(program_);
    }
}

bool Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success = 0;
    GLchar infoLog[1024] = { 0 };
    if (type != "PROGRAM") {
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success) {
            GL_CALL(glGetShaderInfoLog(shader, 1024, nullptr, infoLog));
            errorf("SHADER_COMPILATION_ERROR of type: %s %s", type.c_str(), infoLog);
            return false;
        }
    } else {
        GL_CALL(glGetProgramiv(shader, GL_LINK_STATUS, &success));
        if (!success) {
            GL_CALL(glGetProgramInfoLog(shader, 1024, nullptr, infoLog));
            errorf("PROGRAM_LINKING_ERROR of type: %s %s", type.c_str(), infoLog);
            return false;
        }
    }
    return true;
}

bool Shader::loadShader(const char* vertexShaderCode, const char* fragmentShaderCode) {
    int maxVertexUniform, maxFragmentUniform;
    GL_CALL(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniform));
    GL_CALL(glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniform));
    //infof("maxVertexUniform:%d, maxFragmentUniform:%d\n", maxVertexUniform, maxFragmentUniform);

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GL_CALL(glShaderSource(vertex, 1, &vertexShaderCode, nullptr));
    GL_CALL(glCompileShader(vertex));
    if (!checkCompileErrors(vertex, "VERTEX")) {
        errorf("check compile VERTEX error\n");
        return false;
    }
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CALL(glShaderSource(fragment, 1, &fragmentShaderCode, nullptr));
    GL_CALL(glCompileShader(fragment));
    if (!checkCompileErrors(fragment, "FRAGMENT")) {
        errorf("check compile FRAGMENT error\n");
        return false;
    }
    program_ = glCreateProgram();
    GL_CALL(glAttachShader(program_, vertex));
    GL_CALL(glAttachShader(program_, fragment));
    GL_CALL(glLinkProgram(program_));
    if (!checkCompileErrors(program_, "PROGRAM")) {
        errorf("check compile PROGRAM error\n");
        return false;
    }
    GL_CALL(glDeleteShader(vertex));
    GL_CALL(glDeleteShader(fragment));
    return true;
}

void Shader::use() const {
    GL_CALL(glUseProgram(program_));
}

GLuint Shader::id() const {
    return program_;
}

void Shader::setUniformBool(const std::string& name, bool value) const {
    GL_CALL(glUniform1i(glGetUniformLocation(program_, name.c_str()), (int)value));
}

void Shader::setUniformInt(const std::string& name, int value) const {
    GL_CALL(glUniform1i(glGetUniformLocation(program_, name.c_str()), value));
}

void Shader::setUniformFloat(const std::string& name, float value) const {
    GL_CALL(glUniform1f(glGetUniformLocation(program_, name.c_str()), value));
}

void Shader::setUniformVec2(const std::string& name, const glm::vec2& value) const {
    GL_CALL(glUniform2fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]));
}

void Shader::setUniformVec2(const std::string& name, float x, float y) const {
    GL_CALL(glUniform2f(glGetUniformLocation(program_, name.c_str()), x, y));
}

void Shader::setUniformVec3(const std::string& name, const glm::vec3& value) const {
    GL_CALL(glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]));
}

void Shader::setUniformVec3(const std::string& name, float x, float y, float z) const {
    GL_CALL(glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z));
}

void Shader::setUniformVec4(const std::string& name, const glm::vec4& value) const {
    GL_CALL(glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]));
}

void Shader::setUniformVec4(const std::string& name, float x, float y, float z, float w) const {
    GL_CALL(glUniform4f(glGetUniformLocation(program_, name.c_str()), x, y, z, w));
}

void Shader::setUniformMat2(const std::string& name, const glm::mat2& mat) const {
    GL_CALL(glUniformMatrix2fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]));
}

void Shader::setUniformMat3(const std::string& name, const glm::mat3& mat) const {
    GL_CALL(glUniformMatrix3fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]));
}

void Shader::setUniformMat4(const std::string& name, const glm::mat4& mat) const {
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]));
}

GLuint Shader::getAttribLocation(const std::string& name) const {
    return glGetAttribLocation(program_, name.c_str());
}

GLuint Shader::getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(program_, name.c_str());
}

}
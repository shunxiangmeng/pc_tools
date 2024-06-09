/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  text.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-09 14:43:44
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <map>
#include <memory>
#include "shader.h"
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace playsdk {

typedef struct {
    GLuint texture_id;
    uint32_t bitmap_width;
    uint32_t bitmap_rows;
    FT_Int bitmap_left;
    FT_Int bitmap_top;
    long advance;
} Word;

class Text {
public:
    Text();
    ~Text();
    bool initialize();
    bool render(const glm::mat4& model, const wchar_t* text, int32_t length, const glm::vec3& color);
private:
    bool initShader();
    void loadFaces(const wchar_t* text, int32_t length);
private:
    std::shared_ptr<Shader> shader_;
    std::map<int32_t, Word> word_map_;
    GLuint VAO_;
    GLuint VBO_;
};

}
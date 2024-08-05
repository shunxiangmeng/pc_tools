/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  text.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-09 14:43:31
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "text.h"
#include <vector>
#include "infra/include/Logger.h"
#include "utils.h"

namespace playsdk {

Text::Text() {
}

Text::~Text() {
}

bool Text::initShader() {
    if (shader_) {
        return true;
    }
    shader_ = std::make_shared<Shader>();

    const char* vertex_shader_glsl = R"_(
        #version 330 core
        attribute vec3 position;
        attribute vec2 coordinate;
        uniform mat4 model;
        varying vec2 TexCoords;
        void main()
        {
            TexCoords = coordinate;
            gl_Position = model * vec4(position, 1.0);
        }
    )_";

    const char* fragment_shader_glsl = R"_(
        #version 330 core
        precision mediump float;
        varying vec2 TexCoords;
        uniform vec3 textColor;
        uniform sampler2D texture;
        void main()
        {
            vec4 color = vec4(1.0, 1.0, 1.0, texture(texture, TexCoords).r);
            gl_FragColor = vec4(textColor, 1.0) * color;
        }
    )_";

    if (!shader_->loadShader(vertex_shader_glsl, fragment_shader_glsl)) {
        shader_.reset();
        return false;
    }
    return true;
}

void Text::loadFaces(const wchar_t* text, int32_t length) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        errorf("initialize freetype error");
        return;
    }
    FT_Face face;
    std::string font_file = "arial.ttf";
    //std::string font_file = "Alibaba-PuHuiTi-Regular.ttf";
    FT_Error ret = FT_New_Face(ft, font_file.data(), 0, &face);
    if (ret != 0) {
        errorf("FT_New_Face error, ret:%d\n", ret);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 64);
    FT_Select_Charmap(face, ft_encoding_unicode);

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    for (int32_t i = 0; i < length; ++i) {
        wchar_t ch = text[i];
        auto it = word_map_.find(ch);
        if (it != word_map_.end()) {
            continue;
        }

        if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
            errorf("TextRenderSample::LoadFacesByUnicode FREETYTPE: Failed to load Glyph");
            continue;
        }

        GLuint texture;
        GL_CALL(glGenTextures(1, &texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
        GL_CALL(glTexImage2D(
            GL_TEXTURE_2D, 
            0, 
            GL_RED, 
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0, 
            GL_RED, 
            GL_UNSIGNED_BYTE, 
            face->glyph->bitmap.buffer));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        Word word{texture, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x };
        word_map_[ch] = word;

        //debugf("bitmap.width:%2d, bitmap.rows:%d, bitmap_left:%d, bitmap_top:%d, advance.x:%d\n", 
        //    face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x);
    }
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

bool Text::initialize() {
    if (!initShader()) {
        return false;
    }
    const wchar_t texts[] = L"-0123456789";
    loadFaces(texts, sizeof(texts) / sizeof(texts[0]) - 1);

    GL_CALL(glGenVertexArrays(1, &VAO_));
    GL_CALL(glGenBuffers(1, &VBO_));

    GLuint position = shader_->getAttribLocation("position");
    GLuint coordinate = shader_->getAttribLocation("coordinate");

    GL_CALL(glBindVertexArray(VAO_));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, nullptr, GL_DYNAMIC_DRAW));
    GL_CALL(glEnableVertexAttribArray(position));
    GL_CALL(glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0));
    GL_CALL(glEnableVertexAttribArray(coordinate));
    GL_CALL(glVertexAttribPointer(coordinate, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float))));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, GL_NONE));
    GL_CALL(glBindVertexArray(GL_NONE));
    return true;
}

bool Text::render(const glm::mat4& model, const wchar_t* text, int32_t length, const glm::vec3& color, int32_t x, int32_t y) {
    shader_->use();
    shader_->setUniformMat4("model", model);
    shader_->setUniformVec3("textColor", color);

    GL_CALL(glBindVertexArray(VAO_));
    GL_CALL(glActiveTexture(GL_TEXTURE0));

    float scale = 0.0015f;
    float xpos = x;
    float ypos = y;
    for (int32_t i = 0; i < length; ++i) {
        wchar_t ch = text[i];
        if (ch == L' ') {
            xpos += 32 * scale;
            continue;
        }
        auto it = word_map_.find(ch);
        if (it == word_map_.end()) {
            loadFaces(text + i, length - i);
            i--;
            continue;
        } else {
            Word word = it->second;
            GLfloat w = word.bitmap_width * scale;
            GLfloat h = word.bitmap_top * scale;
            xpos += word.bitmap_left * scale;
            GLfloat vertices[][5] = {
                { xpos,     ypos + h, 0.0,   0.0, 0.0 },
                { xpos,     ypos,     0.0,   0.0, 1.0 },
                { xpos + w, ypos,     0.0,   1.0, 1.0 },

                { xpos,     ypos + h, 0.0,   0.0, 0.0 },
                { xpos + w, ypos,     0.0,   1.0, 1.0 },
                { xpos + w, ypos + h, 0.0,   1.0, 0.0 }
            };

            GL_CALL(glBindTexture(GL_TEXTURE_2D, word.texture_id));
            //glUniform1i(m_SamplerLoc, 0);
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO_));
            GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
            GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
            GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
            xpos += ((word.advance >> 6) * scale * 0.9);
        }
    }
    GL_CALL(glBindVertexArray(0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return true;
}

}
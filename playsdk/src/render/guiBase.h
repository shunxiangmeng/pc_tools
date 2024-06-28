/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  guiBase.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-28 11:17:29
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <stdint.h>
#include <memory>
#include "imgui/imgui.h"
#include "shader.h"

namespace playsdk {
class GuiBase {
public:
    static GuiBase& instance();
    ~GuiBase();
    bool initialize();
    void render();
private:
    GuiBase();
    void initShader();
    void setupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object);
    bool renderDrawData(ImDrawData* draw_data);
private:
    ImGuiContext* mImguiContext;
    GLuint mFontTexture;
    GLuint mShaderHandle;
    GLint  mAttribLocationTex;       // Uniforms location
    GLint  mAttribLocationProjMtx;
    GLuint mAttribLocationVtxPos;    // Vertex attributes location
    GLuint mAttribLocationVtxUV;
    GLuint mAttribLocationVtxColor;
    uint32_t mVboHandle, mElementsHandle;
    GLsizeiptr mVertexBufferSize;
    GLsizeiptr mIndexBufferSize;
    bool mHasClipOrigin;
    bool mUseBufferSubData;
    std::shared_ptr<Shader> mShader;
};
}

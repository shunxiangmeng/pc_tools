/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  shaders.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-05 20:50:16
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "shaders.h"
namespace playsdk {
const char* g_vertex_shader = {
   "attribute vec4 vertexIn;    \
    attribute vec2 textureIn;   \
    varying vec2 textureOut;    \
    void main(void)             \
    {                           \
        gl_Position = vertexIn; \
        textureOut = textureIn; \
    }"
};

const char* g_fragment_shader = {
    "varying vec2 textureOut;          \
    uniform sampler2D tex_y;           \
    uniform sampler2D tex_u;           \
    uniform sampler2D tex_v;           \
    void main(void)                    \
    {                                  \
        vec3 yuv;                      \
        vec3 rgb;                      \
        yuv.x = texture2D(tex_y, textureOut).r;         \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5;   \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5;   \
        rgb = mat3( 1,       1,         1,              \
                    0,       -0.39465,  2.03211,        \
                    1.13983, -0.58060,  0) * yuv;       \
        gl_FragColor = vec4(rgb, 1);                    \
    }"
};
}
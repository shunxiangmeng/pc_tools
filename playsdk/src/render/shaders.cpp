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
   "#version 330 core\n                      \
    attribute vec3 position;                 \
    attribute vec2 coordniate;               \
    varying vec2 textureOut;                 \
    void main(void)                          \
    {                                        \
        gl_Position = vec4(position.x, position.y, position.z, 1.0);;             \
        textureOut = coordniate;             \
    }"
};

const char* g_fragment_shader = {
   "#version 330 core\n                                  \
    varying vec2 textureOut;                             \
    uniform sampler2D tex_y;                             \
    uniform sampler2D tex_u ;                            \
    uniform sampler2D tex_v ;                            \
    void main(void)                                      \
    {                                                    \
        vec3 yuv;                                        \
        vec3 rgb;                                        \
        yuv.r = texture2D(tex_y, textureOut).r;          \
        yuv.g = texture2D(tex_u, textureOut).r - 0.5;    \
        yuv.b = texture2D(tex_v, textureOut).r - 0.5;    \
        rgb = mat3( 1.0,     1.0,       1.0,             \
                    0.0,     -0.21482,  2.12798,         \
                    1.28033, -0.38059,  0.0) * yuv;      \
        gl_FragColor = vec4(rgb, 1);                     \
    }"
};
}
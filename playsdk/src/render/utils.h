/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  utils.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-07 14:44:10
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "infra/include/Logger.h"

#define OPENGL_DEBUG
#ifdef OPENGL_DEBUG
#define GL_CALL(_CALL)  do { _CALL; GLenum gl_err = glGetError(); if (gl_err != 0) errorf("GL error %d returned from '%s'\n", gl_err,#_CALL); } while (0)
#else
#define GL_CALL(_CALL)
#endif
namespace playsdk {

typedef struct {
    float x;
    float y;
    float z;
}Position;

typedef struct {
    float x;
    float y;
}Coordinate;

typedef struct {
    Position position;
    Coordinate texCoords;
}SampleVertex2D;

typedef struct {
    Position position;
    Coordinate texCoords0;
    Coordinate texCoords1;
}SampleVertex3D;

}
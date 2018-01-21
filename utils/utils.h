#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <GLXW/glxw.h>
#include <vector>
#include "stb_image.h"

GLuint prepareProgram(const std::vector<GLuint>& shaders, bool *errorFlagPtr);
GLuint loadShader(const char* fileName, GLenum shaderType, bool *errorFlagPtr);

bool loadCommonTexture(const char* fname, GLuint textureId);
bool loadCommonTextureExt(const char* fname, GLuint textureId, bool flip);

#endif // OPENGL_UTILS_H

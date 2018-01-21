#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <GLXW/glxw.h>
#include <vector>

GLuint prepareProgram(const std::vector<GLuint>& shaders, bool *errorFlagPtr);
GLuint loadShader(const char* fileName, GLenum shaderType, bool *errorFlagPtr);

#endif // OPENGL_UTILS_H

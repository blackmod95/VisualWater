#ifndef AFISKON_OPENGL_UTILS_H
#define AFISKON_OPENGL_UTILS_H

#include <GLXW/glxw.h>
#include <vector>

GLuint prepareProgram(const std::vector<GLuint>& shaders, bool *errorFlagPtr);
GLuint loadShader(const char* fileName, GLenum shaderType, bool *errorFlagPtr);

#endif // AFISKON_OPENGL_UTILS_H

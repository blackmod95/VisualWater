#include "utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GLFW/glfw3.h>

bool checkShaderCompileStatus(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log((unsigned long)length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return true;
    }
    return false;
}

bool checkProgramLinkStatus(GLuint obj) {
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log((unsigned long) length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return true;
    }
  return false;
}

GLuint loadShader(const char* fileName, GLenum shaderType, bool *errorFlagPtr) {
    std::ifstream fileStream(fileName);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    std::string shaderSource(buffer.str());

    GLuint shaderId = glCreateShader(shaderType);
    const GLchar * const shaderSourceCStr = shaderSource.c_str();

    glShaderSource(shaderId, 1, &shaderSourceCStr, nullptr);
    glCompileShader(shaderId);

    *errorFlagPtr = checkShaderCompileStatus(shaderId);
    if(*errorFlagPtr) {
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

GLuint prepareProgram(const std::vector<GLuint>& shaders, bool *errorFlagPtr) {
    *errorFlagPtr = false;

    GLuint programId = glCreateProgram();
    for(auto it = shaders.cbegin(); it != shaders.cend(); ++it) {
        glAttachShader(programId, *it);
    }
    glLinkProgram(programId);

    *errorFlagPtr = checkProgramLinkStatus(programId);
    if(*errorFlagPtr) {
        glDeleteProgram(programId);
        return 0;
    }

    return programId;
}

void flipTexture(unsigned char *textureData, int width, int height, int n) {
    for(int h = 0; h < height/2; ++h) {
        for(int w = 0; w < width; ++w) {
            for(int i = 0; i < n; ++i) {
                int offset1 = (h*width + w)*n + i;
                int offset2 = ((height - h - 1)*width + w)*n + i;
                unsigned char t = textureData[offset1];
                textureData[offset1] = textureData[offset2];
                textureData[offset2] = t;
            }
        }
    }
}

bool loadCommonTexture(char const* fname, GLuint textureId) {
    return loadCommonTextureExt(fname, textureId, false);
}

bool loadCommonTextureExt(char const* fname, GLuint textureId, bool flip) {
    int width, height, n;
    unsigned char *textureData = stbi_load(fname, &width, &height, &n, 3);
    if(textureData == nullptr) {
        std::cout << "ERROR: loadCommonTextureExt failed, fname = " << fname << ", stbi_load returned nullptr" << std::endl;
        return false;
    }

    if(flip) flipTexture(textureData, width, height, n);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glBindTexture(GL_TEXTURE_2D, 0); // unbind
    return true;
}
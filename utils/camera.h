#ifndef AFISKON_OPENGL_CAMERA_H
#define AFISKON_OPENGL_CAMERA_H

#include <glm/core/type.hpp>

class Camera {

private:
    GLFWwindow*window;
    glm::vec3 position;
    float horizontalAngleRad;
    float verticalAngleRad;
    float radius;

public:
    Camera(GLFWwindow* window, const glm::vec3&startPosition, float startHorizontalAngleRad, float startVerticalAngleRad, float startRadius);
    void getViewMatrix(float deltaTimeMs, glm::mat4* pOutViewMatrix);

    Camera(Camera const &) = delete;
    void operator=(Camera const &x) = delete;
};

#endif // AFISKON_OPENGL_CAMERA_H

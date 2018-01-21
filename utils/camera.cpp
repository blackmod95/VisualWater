#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "camera.h"

static const float speed = 4.0f;

Camera::Camera(GLFWwindow* window, float startHorizontalAngleRad, float startVerticalAngleRad, float startRadius):
    window(window),
    horizontalAngleRad(startHorizontalAngleRad),
    verticalAngleRad(startVerticalAngleRad),
    radius(startRadius)
    { }

glm::vec3 Camera::getPosition()
{
    return position;
}

void Camera::getViewMatrix(float deltaTimeMs, glm::mat4* pOutViewMatrix) {
    float deltaTimeSec = deltaTimeMs/1000.0f;

    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.3f);
    position = radius * glm::vec3(
            sin(verticalAngleRad) * cos(horizontalAngleRad),
            sin(verticalAngleRad) * sin(horizontalAngleRad),
            cos(verticalAngleRad)) + direction;

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        verticalAngleRad += deltaTimeSec * speed;
        verticalAngleRad = std::min(verticalAngleRad, 0.0f - 3.14f/128);
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        verticalAngleRad -= deltaTimeSec * speed;
        verticalAngleRad = std::max(verticalAngleRad, -3.14f + 3.14f/12);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngleRad += deltaTimeSec * speed;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngleRad -= deltaTimeSec * speed;
    }

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        radius -= deltaTimeSec * speed;
        radius = std::max(radius, 0.01f);
    }

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        radius += deltaTimeSec * speed;
    }

    *pOutViewMatrix = glm::lookAt(position, direction, glm::vec3(0,0,1));
}
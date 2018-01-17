#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "camera.h"

static const float speed = 4.0f; // units per second
static const float mouseSpeedRad = 0.0005f;

Camera::Camera(GLFWwindow* window, const glm::vec3& startPosition, float startHorizontalAngleRad, float startVerticalAngleRad, float startRadius):
    window(window),
    position(startPosition),
    horizontalAngleRad(startHorizontalAngleRad),
    verticalAngleRad(startVerticalAngleRad),
    radius(startRadius)
    { }

void Camera::getViewMatrix(float deltaTimeMs, glm::mat4* pOutViewMatrix) {
    float deltaTimeSec = deltaTimeMs/1000.0f;

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    //horizontalAngleRad += mouseSpeedRad * (windowWidth/2 - mouseX);
    //verticalAngleRad += mouseSpeedRad * (windowHeight/2 - mouseY);

    glfwSetCursorPos(window, windowWidth/2, windowHeight/2);

    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.2f);

    glm::vec3 right = glm::vec3(
        sin(horizontalAngleRad - 3.14f/2.0f),
        0,
        cos(horizontalAngleRad - 3.14f/2.0f)
    );

    glm::vec3 up = glm::cross(right, direction);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        verticalAngleRad += deltaTimeSec * speed;
        verticalAngleRad = std::min(verticalAngleRad, 0.0f - 3.14f/6);
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        verticalAngleRad -= deltaTimeSec * speed;
        verticalAngleRad = std::max(verticalAngleRad, -3.14f + 3.14f/3);
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngleRad += deltaTimeSec * speed;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngleRad -= deltaTimeSec * speed;
    }

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        radius -= deltaTimeSec * speed;
        radius = std::max(radius, 0.1f);
    }

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        radius += deltaTimeSec * speed;
    }

    position = radius * glm::vec3(
            sin(verticalAngleRad) * cos(horizontalAngleRad),
            sin(verticalAngleRad) * sin(horizontalAngleRad),
            cos(verticalAngleRad)) + direction;

    //*pOutViewMatrix = glm::lookAt(position, position + direction, up);
    *pOutViewMatrix = glm::lookAt(position, direction, glm::vec3(0,0,1));
}
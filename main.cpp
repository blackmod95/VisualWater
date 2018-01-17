#include <iostream>
#include <vector>
#include <chrono>

#include <GLXW/glxw.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "utils/utils.h"
#include "utils/camera.h"

static const int sideSize = 100;
static GLfloat globVertexBufferData[3 * sideSize * sideSize];

void windowSizeCallback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if(glfwInit() == GL_FALSE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 500;
    int height = 500;

    GLFWwindow* window = glfwCreateWindow(width, height, "Visual water",
                                          nullptr, nullptr);
    if(window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glxwInit()) {
        std::cerr << "Failed to init GLXW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwShowWindow(window);

    glEnable(GL_DOUBLEBUFFER);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 1);

    bool errorFlag = false;
    std::vector<GLuint> shaders;

    GLuint vertexShaderId = loadShader("shaders/vertexShader.glsl", GL_VERTEX_SHADER, &errorFlag);
    if(errorFlag) {
        std::cerr << "Failed to load vertex shader (invalid working directory?)" << std::endl;
        return -1;
    }

    shaders.push_back(vertexShaderId);

    GLuint fragmentShaderId = loadShader("shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER, &errorFlag);
    if(errorFlag) {
        std::cerr << "Failed to load fragment shader (invalid working directory?)" << std::endl;
        return -1;
    }
    shaders.push_back(fragmentShaderId);

    GLuint programId = prepareProgram(shaders, &errorFlag);
    if(errorFlag) {
        std::cerr << "Failed to prepare program" << std::endl;
        return -1;
    }

    // инициализация массива с вершинами будущей поверхности
    for (int i = 0; i < sideSize; i++)
    {
        for (int j = 0; j < sideSize; j++)
        {
            globVertexBufferData[3 * (i * sideSize + j) + 0] = -1.0f + 2.0f * static_cast<float>(i) / (sideSize - 1);
            globVertexBufferData[3 * (i * sideSize + j) + 1] = -1.0f + 2.0f * static_cast<float>(j) / (sideSize - 1);
            globVertexBufferData[3 * (i * sideSize + j) + 2] = 0;
        }
    }

    for (int i = 0; i < sideSize*sideSize; i++)
    {
        std::cout << globVertexBufferData[3*i+0] << " " << globVertexBufferData[3*i+1] << " " << globVertexBufferData[3*i+2] << std::endl;
    }

    GLuint vboVertex;
    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(globVertexBufferData), globVertexBufferData, GL_STREAM_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO
    glBindVertexArray(0); // unbind VAO

    glm::mat4 projection = glm::perspective(120.0f, (float)width / (float)height, 0.3f, 100.0f);

    GLint matrixId = glGetUniformLocation(programId, "MVP");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera camera(window, glm::vec3(-0.01f, -0.01f , 1.0f), 0.0f, 0.0, 2.0f);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    glPointSize(2);

    auto prevTimeForUpdateArrays = startTime;

    while(glfwWindowShouldClose(window) == GL_FALSE) {
        // выход из программы
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) break;

        // отображение ребер полигонов
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        // окрашенное изображение
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        float startDeltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        float prevDeltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count();
        float prevDeltaTimeForUpdateMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTimeForUpdateArrays).count();
        prevTime = currentTime;

        if (prevDeltaTimeForUpdateMs > 1000.0f/24)
        {
            prevTimeForUpdateArrays = currentTime;
            for (int i = 0; i < sideSize; i++)
            {
                for (int j = 0; j < sideSize; j++)
                {
                    GLfloat x = globVertexBufferData[3 * (i * sideSize + j) + 0];
                    GLfloat y = globVertexBufferData[3 * (i * sideSize + j) + 1];
                    globVertexBufferData[3 * (i * sideSize + j) + 2] = 0.03f * std::cos(10.0f*x+10.1f*y+5.0f*(startDeltaTimeMs/1000));
                }
            }
            glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(globVertexBufferData), globVertexBufferData);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glm::mat4 view;
        camera.getViewMatrix(prevDeltaTimeMs, &view);

        glm::mat4 model = glm::rotate(0.0f, 1.0f, 0.0f, 0.0f);

        glm::mat4 mvp = projection * view * model; // matrix multiplication is the other way around
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Растягивание изображения в зависимости от размера окна
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, 2*width, 2*height);

        glUseProgram(programId); // Использование шейдеров

        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_POINTS, 0, sideSize * sideSize);

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboVertex);
    glDeleteProgram(programId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
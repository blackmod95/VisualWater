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
static const int globVertexBufferDataSize = 2 * 3 * 3 * (sideSize-1) * (sideSize-1);
static GLfloat globVertexBufferData[globVertexBufferDataSize];

struct Wave
{
    GLfloat ampl;
    GLfloat vec_x;
    GLfloat vec_y;
    GLfloat freq;
    GLfloat phase;
};

std::vector<Wave> waves;

void windowSizeCallback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
}

void init()
{
    // триангуляция водной поверхности
    for (int i = 0; i < sideSize - 1; i++)
    {
        for (int j = 0; j < sideSize - 1; j++)
        {
            // текущая вершина
            GLfloat x00 = -1.0f + 2.0f * static_cast<float>(i) / (sideSize - 1);
            GLfloat y00 = -1.0f + 2.0f * static_cast<float>(j) / (sideSize - 1);
            GLfloat x01 = -1.0f + 2.0f * static_cast<float>(i) / (sideSize - 1);
            GLfloat y01 = -1.0f + 2.0f * static_cast<float>(j+1) / (sideSize - 1);
            GLfloat x10 = -1.0f + 2.0f * static_cast<float>(i+1) / (sideSize - 1);
            GLfloat y10 = -1.0f + 2.0f * static_cast<float>(j) / (sideSize - 1);
            GLfloat x11 = -1.0f + 2.0f * static_cast<float>(i+1) / (sideSize - 1);
            GLfloat y11 = -1.0f + 2.0f * static_cast<float>(j+1) / (sideSize - 1);

            // на основании 4 вершин строится 2 треугольника. Добавляем их как 6 разных вершин
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 0] = x00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 1] = y00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 2] = 0;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 3] = x01;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 4] = y01;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 5] = 0;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 6] = x11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 7] = y11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 8] = 0;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 9] = x00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 10] = y00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 11] = 0;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 12] = x11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 13] = y11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 14] = 0;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 15] = x10;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 16] = y10;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 17] = 0;
        }
    }

    // волны задаются в отдельном векторе
    Wave wave1, wave2, wave3;
    wave1.ampl = 0.05f;
    wave1.vec_x = 10.0f;
    wave1.vec_y = 10.0f;
    wave1.freq = 5.0f;
    wave1.phase = 0.0f;
    waves.push_back(wave1);
    wave2.ampl = 0.06f;
    wave2.vec_x = 12.0f;
    wave2.vec_y = 12.0f;
    wave2.freq = 2.0f;
    wave2.phase = 3.14f/2;
    waves.push_back(wave2);
    wave3.ampl = 0.03f;
    wave3.vec_x = 0.0f;
    wave3.vec_y = 3.0f;
    wave3.freq = 2.0f;
    wave3.phase = 0.75f;
    waves.push_back(wave3);
}

void update(GLuint &vboVertex, float &startDeltaTimeMs)
{
    for (int i = 0; i < globVertexBufferDataSize / 3; i++)
    {
        GLfloat x = globVertexBufferData[3 * i + 0];
        GLfloat y = globVertexBufferData[3 * i + 1];
        globVertexBufferData[3 * i + 2] = 0.0f;
        for (int w = 0; w < waves.size(); w++)
        {
            globVertexBufferData[3 * i + 2] +=
                    waves[w].ampl * std::cos(waves[w].vec_x * x + waves[w].vec_y * y +
                    waves[w].freq * (startDeltaTimeMs / 1000) + waves[w].phase);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(globVertexBufferData), globVertexBufferData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    init();

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

    glm::mat4 projection = glm::perspective(80.0f, (float)width / (float)height, 0.3f, 100.0f);

    GLint matrixId = glGetUniformLocation(programId, "MVP");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera camera(window, 0.0f, 0.0, 2.0f);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

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
            update(vboVertex, startDeltaTimeMs);
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
        glDrawArrays(GL_TRIANGLES, 0, 18*(sideSize-1) * (sideSize-1));

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
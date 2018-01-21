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

#define STBI_NO_GIF 1
#define STB_IMAGE_IMPLEMENTATION 1
#include "stb/stb_image.h"

static const int sideSize = 100;
static const int vertexAmount = 2 * 3 * (sideSize-1) * (sideSize-1);
static const int globVertexBufferDataSize = 3 * vertexAmount;
static const int globNormalBufferDataSize = 3 * vertexAmount;
static GLfloat globVertexBufferData[globVertexBufferDataSize];
static GLfloat globNormalBufferData[globNormalBufferDataSize];


struct Wave
{
    GLfloat ampl;
    GLfloat vec_x;
    GLfloat vec_y;
    GLfloat freq;
    GLfloat phase;

    Wave(GLfloat _ampl, GLfloat _vec_x, GLfloat _vec_y ,GLfloat _freq ,GLfloat _phase)
    {
        ampl = _ampl;
        vec_x = _vec_x;
        vec_y = _vec_y;
        freq = _freq;
        phase = _phase;
    }
};

std::vector<Wave> waves;

void windowSizeCallback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
}

GLfloat height(const std::vector<Wave> &waves, GLfloat x, GLfloat y, GLfloat time)
{
    GLfloat result = 0.0f;

    for (int w = 0; w < waves.size(); w++) {
        result += waves[w].ampl * std::cos(waves[w].vec_x * x + waves[w].vec_y * y + waves[w].freq * time + waves[w].phase);
    }

    return result;
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
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 2] = 0.0f;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 3] = x01;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 4] = y01;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 5] = 0.0f;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 6] = x11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 7] = y11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 8] = 0.0f;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 9] = x00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 10] = y00;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 11] = 0.0f;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 12] = x11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 13] = y11;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 14] = 0.0f;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 15] = x10;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 16] = y10;
            globVertexBufferData[18 * (i * (sideSize - 1) + j) + 17] = 0.0f;
        }
    }

    // волны задаются в отдельном векторе
    waves.push_back(Wave(0.05f, 20.0f, 0.0f, 5.0f, 0.0f));
    waves.push_back(Wave(0.03f, 0.0f, 10.0f, 8.0f, 1.0f));
    //waves.push_back(Wave(0.0f, 20.0f, 0.0f, 5.0f, 0.0f));

    for (int i = 0; i < globNormalBufferDataSize; i++)
    {
        globNormalBufferData[i] = 0.0f;
    }
}

void update(GLuint &vboVertex, GLuint &vboNormal, GLfloat &startDeltaTimeMs) {
    // Меняем высоту водной поверхности в заданной точке. Считается как сумма волн.
    GLfloat time = startDeltaTimeMs / 1000;

    for (int i = 0; i < globVertexBufferDataSize / 3; i++) {
        GLfloat x = globVertexBufferData[3 * i + 0];
        GLfloat y = globVertexBufferData[3 * i + 1];
        globVertexBufferData[3 * i + 2] = height(waves, x, y, time);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(globVertexBufferData), globVertexBufferData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // После обновления массива координат вершин пересчитываем нормали.
    for (int i = 0; i < globVertexBufferDataSize / 3; i++) {
        GLfloat x = globVertexBufferData[3 * i + 0];
        GLfloat y = globVertexBufferData[3 * i + 1];
        GLfloat x_angle = 0.0f;
        GLfloat y_angle = 0.0f;
        for (int w = 0; w < waves.size(); w++) {
            GLfloat diff = -waves[w].ampl * std::sin(waves[w].vec_x * x + waves[w].vec_y * y +
                                                     waves[w].freq * time + waves[w].phase);
            x_angle += waves[w].vec_x * diff;
            y_angle += waves[w].vec_y * diff;
        }
        glm::vec3 x_tan_vec(1.0f, 0.0f, x_angle);
        glm::vec3 y_tan_vec(0.0f, 1.0f, y_angle);
        glm::vec3 cross_xy = glm::cross(x_tan_vec, y_tan_vec);

        globNormalBufferData[3 * i + 0] = cross_xy.x;
        globNormalBufferData[3 * i + 1] = cross_xy.y;
        globNormalBufferData[3 * i + 2] = cross_xy.z;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(globNormalBufferData), globNormalBufferData);
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
    glEnable(GL_CULL_FACE);
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

    GLuint vboVertex, vboNormal;
    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(globVertexBufferData), globVertexBufferData, GL_STREAM_DRAW);

    glGenBuffers(1, &vboNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
    glBufferData(GL_ARRAY_BUFFER, sizeof(globVertexBufferData), globVertexBufferData, GL_STREAM_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(80.0f, (float)width / (float)height, 0.3f, 100.0f);

    glm::vec3 sunDir(1.0f, 0.0f, 50.0f);
    glm::vec3 sunColor(0.7f, 0.7f, 0.0f);
    glm::vec3 ambColor(0.1f, 0.1f, 0.5f);

    GLint matrixId = glGetUniformLocation(programId, "MVP");
    GLint eyeId = glGetUniformLocation(programId, "eye");
    GLint samplerId = glGetUniformLocation(programId, "textureSampler");
    GLint sunDirId = glGetUniformLocation(programId, "sunDir");
    GLint sunColorId = glGetUniformLocation(programId, "sunColor");
    GLint ambColorId = glGetUniformLocation(programId, "ambColor");

    std::cout << "matrixId = " << matrixId << std::endl;
    std::cout << "eyeId = " << eyeId << std::endl;
    std::cout << "textureSampler = " << samplerId << std::endl;
    std::cout << "sunDirId = " << sunDirId << std::endl;
    std::cout << "sunColorId = " << sunColorId << std::endl;
    std::cout << "ambColorId = " << ambColorId << std::endl;

    glUseProgram(programId);
    glUniform3f(sunDirId, sunDir.x, sunDir.y, sunDir.z);
    glUniform3f(sunColorId, sunColor.x, sunColor.y, sunColor.z);
    glUniform3f(ambColorId, ambColor.x, ambColor.y, ambColor.z);
    glUniform1i(samplerId, 0);
    glUseProgram(0);

    // Текстуры

    GLuint textureArray[2];
    int texturesNum = sizeof(textureArray)/sizeof(textureArray[0]);
    glGenTextures(texturesNum, textureArray);

    if (!loadCommonTexture("textures/sky_texture.jpg", textureArray[0]))
        return -1;

    glUseProgram(programId);
    glBindTexture(GL_TEXTURE_2D, textureArray[0]);
    glUseProgram(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera camera(window, 0.0f, -0.5f, 2.0f);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    auto prevTimeForUpdateArrays = startTime;

    // Основной цикл. Не завершится, пока не нажмешь на Q.
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
            update(vboVertex, vboNormal, startDeltaTimeMs);
        }

        glm::mat4 view;
        camera.getViewMatrix(prevDeltaTimeMs, &view);

        glm::mat4 model = glm::rotate(180.0f, 1.0f, 0.0f, 0.0f);

        glm::mat4 mvp = projection * view * model;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Растягивание изображения в зависимости от размера окна
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, 2*width, 2*height);

        glm::vec3 eye = camera.getPosition();

        glUseProgram(programId); // Использование шейдеров
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, &mvp[0][0]);
        glUniform3f(eyeId, eye.x, eye.y, eye.z);

        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 18*(sideSize-1) * (sideSize-1));
        glEnableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glUseProgram(0);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboVertex);
    glDeleteBuffers(1, &vboNormal);
    glDeleteProgram(programId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
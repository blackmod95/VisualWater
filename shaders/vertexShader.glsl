#version 330 core

layout(location = 0) in vec3 vertexPos;

out vec3 fragmentColor;

uniform mat4 MVP;

void main() {
    fragmentColor = vec3(0.0f, 0.5f, 0.5f);

    vec4 temp = vec4(vertexPos, 1);
    gl_Position = MVP * temp;
}
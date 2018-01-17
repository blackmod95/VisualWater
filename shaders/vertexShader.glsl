#version 330 core

layout(location = 0) in vec3 vertexPos;

out float warm;

uniform mat4 MVP;

void main() {
    warm = (1 - vertexPos.z) * 0.5;

    vec4 temp = vec4(vertexPos.xy, warm, 1);
    gl_Position = MVP * temp;
}
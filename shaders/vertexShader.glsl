#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;

out vec3 positionTmp;
out vec3 normalTmp;

uniform mat4 MVP;
uniform vec3 eye;

void main() {
    normalTmp = normalize(vertexNormal);
    positionTmp = vertexPos;

    float warm = 1.0f - (1.0f + vertexPos.z)/(1 + eye.z);

    vec4 temp = vec4(vertexPos.xy, vertexPos.z * warm, warm);
    gl_Position = MVP * temp;
}
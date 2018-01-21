#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;

out vec3 positionTmp;
out vec3 normalTmp;

//uniform vec3 sunDir;
uniform mat4 MVP;

void main() {
    normalTmp = normalize(vertexNormal);
    positionTmp = vertexPos;

    float warm = (1.0f - vertexPos.z) * 0.5f;

    vec4 temp = vec4(vertexPos.xy, vertexPos.z * warm, warm);
    gl_Position = MVP * temp;
}
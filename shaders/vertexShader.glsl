#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;

out float dirLight;

uniform vec3 sunDir;
uniform mat4 MVP;

void main() {
    vec3 normal = normalize(vertexNormal);
    vec3 normalSun = normalize(sunDir);
    dirLight = max(0.0f, dot(normal, normalSun));

    float warm = (1.0f - vertexPos.z) * 0.5f;

    vec4 temp = vec4(vertexPos.xy, vertexPos.z * warm, 1.0f);
    gl_Position = MVP * temp;
}
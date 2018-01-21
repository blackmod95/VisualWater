#version 330 core

in float dirLight;

out vec4 color;

uniform vec3 sunColor;
uniform vec3 ambColor;

void main() {
    vec3 rgb = clamp(sunColor * dirLight + ambColor, 0.0f, 1.0f);
    color = vec4(rgb, 1.0f);
}
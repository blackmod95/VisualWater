#version 330 core

in float warm;

out vec4 color;

void main() {
    vec3 rgb = mix(vec3(1,0.5,0), vec3(0,0.5,1), warm);
    color = vec4(rgb,1);
}
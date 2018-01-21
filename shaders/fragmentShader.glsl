#version 330 core

uniform sampler2D textureSampler;
uniform vec3 sunDir;
uniform vec3 ambColor;
uniform vec3 eye;

in vec3 normalTmp;
in vec3 positionTmp;

out vec4 color;

void main() {
    vec3 toEye = normalize(positionTmp - eye);
    vec3 reflected = normalize(toEye - 2*normalTmp*dot(normalTmp, toEye)/ dot(normalTmp, normalTmp));

    float dirLight = max(0.5f, dot(normalTmp, normalize(sunDir)));

    vec2 UVcoord = 0.25*reflected.xy/reflected.z + (0.5, 0.5);
    vec3 skyColor = texture(textureSampler, UVcoord).rgb;

    vec3 rgb = clamp(skyColor * dirLight + ambColor, 0.0f, 1.0f);
    color = vec4(rgb, 1.0f);
}
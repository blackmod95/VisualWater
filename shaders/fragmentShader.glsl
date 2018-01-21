#version 330 core

uniform sampler2D skySampler;
uniform sampler2D sandSampler;

uniform vec3 sunDir;
uniform vec3 eye;
uniform float coefRefr;
uniform float sandDepth;
uniform vec3 sunDiffuseColor;
uniform vec3 sunReflColor;

in vec3 normalTmp;
in vec3 positionTmp;

out vec4 color;

void main() {
    vec3 toEye = normalize(positionTmp - eye);
    vec3 reflected = normalize(toEye - 2*normalTmp*dot(normalTmp, toEye));

    vec2 skyUV = 0.25f*reflected.xy/reflected.z + vec2(0.5f, 0.5f);
    vec3 skyColor = texture(skySampler, skyUV).rgb;

    vec3 cr = cross(normalTmp, toEye);
    float c2 = sqrt(1 - coefRefr * coefRefr * dot(cr, cr));
    vec3 refracted = normalize(coefRefr * cross(cr, normalTmp) - normalTmp * c2);
    float c1 = -dot(normalTmp, toEye);

    float t = (-sandDepth-positionTmp.z)/refracted.z;
    vec3 sandPoint = positionTmp + t * refracted;
    vec2 sandUV = sandPoint.xy + vec2(0.5, 0.5);
    vec3 sandColor = texture(sandSampler, sandUV).rgb;

    float reflectance_s = pow((coefRefr * c1 - c2)/(coefRefr * c1 + c2),2);
    float reflectance_p = pow((coefRefr * c2 - c1)/(coefRefr * c2 + c1),2);
    float reflectance = (reflectance_s + reflectance_p)/2;

    float diw = length(sandPoint - positionTmp);
    vec3 filterWater = vec3(1.0f, 0.5f, 0.2f);
    vec3 mask = vec3(exp(-diw * filterWater.x), exp(-diw * filterWater.y), exp(-diw * filterWater.z));

    float diffuseInt = 0.5f * max(0, dot(normalTmp, sunDir));
    float cosphi = max(0.0f, dot(sunDir,normalize(reflected)));
    float reflInt = pow(cosphi,100);

    vec3 ambWater = vec3(0.0f, 0.3f, 0.5f);
    vec3 imageColor = sandColor * mask + ambWater * (1 - mask);

    vec3 rgb = skyColor * reflectance + imageColor * (1 - reflectance) + diffuseInt * sunDiffuseColor + reflInt * sunReflColor;
    color = vec4(rgb, 1.0f);
}
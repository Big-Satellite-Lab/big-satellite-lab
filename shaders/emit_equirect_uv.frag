#version 460
#define MAX_NUM_TOTAL_LIGHTS 10

layout (location = 0) in vec3 localPos;
layout (location = 1) in vec3 fragPos;
layout (location = 2) in vec3 camPos;
layout (location = 3) in mat3 TBN;
layout (location = 6) in vec3 lightPos[MAX_NUM_TOTAL_LIGHTS];

layout (location = 0) out vec4 outFragColor;

layout (push_constant) uniform PushConstants
{
    vec4 roughness_multiplier; // only x component is used
    mat4 render_matrix;
} constants;

struct Light {
    vec4 position;  // w is unused
    vec4 color;     // w is for intensity
};

layout (set = 0, binding = 1) uniform SceneData {
    vec4 ambientColor;
    vec4 sunDirection;
    vec4 sunColor; // w is for sun power
    vec4 camPos; // w is unused
    Light lights[MAX_NUM_TOTAL_LIGHTS];
    int numLights;
} sceneData;

layout (set = 2, binding = 0) uniform sampler2D diffuseTex;
layout (set = 2, binding = 1) uniform sampler2D normalTex;
layout (set = 2, binding = 2) uniform sampler2D roughnessTex;
layout (set = 2, binding = 3) uniform sampler2D aoTex;
layout (set = 2, binding = 4) uniform sampler2D metalTex;

// diffuse portion of integral
layout (set = 2, binding = 5) uniform samplerCube irradianceMap;
// first portion of specular portion of integral
layout (set = 2, binding = 6) uniform samplerCube prefilterMap;
// second portion of specular portion of integral
layout (set = 2, binding = 7) uniform sampler2D brdfLUT;

const float PI = 3.14159265359;

const vec2 invAtan = vec2(1.0 / (2.0 * PI), 1.0 / PI);

vec2 sampleSphericalMap(vec3 v)
{
    // uv.x is theta in spherical coordinates. it's simply the angle in x-z plane.
    // sin(phi) = opp / hyp = y / 1
    // since v is a unit vector. note that phi is the angle between x-z plane and v, which
    // is different than the wikipedia article.
    // so we see uv.y = phi = asin(y)
    // vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

    // atan gives value in [-pi, pi] and asin gives value in [-pi/2, pi/2]
    // so we normalize to get value in [-0.5, 0.5]
    uv *= invAtan;
    // then shift to get in range [0, 1]
    uv += 0.5;

    return 1.0 - uv;
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(localPos));

    // Remove discontinuity in uv coordinate to avoid artifacts in mipmap
    vec2 uvd = uv;
    uvd.x -= 0.5;
    uvd.x = abs(uvd.x);
    uvd.x += 0.5;

    vec3 diffuse = textureGrad(diffuseTex, uv, dFdxCoarse(uvd), dFdyCoarse(uvd)).rgb;

    outFragColor = vec4(diffuse, 1.0);
}

#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec4 Color;

struct ParticleData
{
    vec4 worldPos;
    vec4 color;
    vec4 scale;
};

layout(std430, binding = 3) buffer Particles
{
    vec4 CameraRight;
    vec4 CameraUp;
    ParticleData particleData[];
};

void main()
{
    vec3 vertexPosWS = particleData[gl_InstanceID].worldPos.xyz
        + CameraRight.xyz * aPos.x * particleData[gl_InstanceID].scale.x
        + CameraUp.xyz * aPos.y * particleData[gl_InstanceID].scale.y;
    gl_Position = projection * view * vec4(vertexPosWS, 1.0);
    TexCoord = aUv;
    Color = particleData[gl_InstanceID].color;
}
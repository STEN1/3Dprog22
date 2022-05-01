#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

struct BillboardData
{
    vec4 uvPosAndScale;
    vec4 worldPos;
};

layout(std430, binding = 2) buffer TextBillboard
{
    BillboardData billboardData[];
};

void main()
{
    gl_Position = projection * view * model * vec4((aPos + billboardData[gl_InstanceID].worldPos.xyz), 1.0);
        TexCoord = billboardData[gl_InstanceID].uvPosAndScale.xy
            + aUv * billboardData[gl_InstanceID].uvPosAndScale.zw;
}
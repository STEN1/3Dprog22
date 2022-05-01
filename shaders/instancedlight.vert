#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aVertexNormal;
layout (location = 3) in mat4 aInstancedMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;
out vec2 textureCoord;
out vec3 vertexNormal;
out vec3 fragPos;

void main()
{
    gl_Position = projection * view * aInstancedMatrix * vec4(aPos, 1.0f);
    fragPos = vec3(aInstancedMatrix * vec4(aPos, 1.0));
    textureCoord = aTextureCoord;
    vertexNormal = mat3(transpose(inverse(aInstancedMatrix))) * aVertexNormal;
}
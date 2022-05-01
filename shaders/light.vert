#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aVertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;
out vec2 textureCoord;
out vec3 vertexNormal;
out vec3 fragPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fragPos = vec3(model * vec4(aPos, 1.0));
    //vertexColor = aVertexColor;
    textureCoord = aTextureCoord;
    vertexNormal = mat3(transpose(inverse(model))) * aVertexNormal;

    // mat3(transpose(inverse(model))) * aNormal;
}
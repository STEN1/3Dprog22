#version 460 core

out vec4 fragmentColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main() {
   fragmentColor = texture(skybox, texCoord);
}

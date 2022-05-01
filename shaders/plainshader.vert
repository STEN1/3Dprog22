#version 460 core

layout(location = 0) in vec4 aPosition;

out vec4 color;      

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 uColor;

void main() {
   gl_Position = projection * view * model * aPosition;
   color = uColor;
}

#version 460 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 color = texture(ourTexture, TexCoord);
    color.a = (color.r + color.g + color.b) / 3.0;
    FragColor = color;
}
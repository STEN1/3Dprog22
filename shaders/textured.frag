#version 460 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 fragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
    if (fragColor.a < 0.1)
        discard;
    FragColor = fragColor;
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
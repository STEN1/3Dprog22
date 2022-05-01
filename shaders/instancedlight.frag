#version 460 core

out vec4 FragColor;
  
in vec2 textureCoord;
in vec3 vertexNormal;
in vec3 fragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct PointLight {    
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    
    float constant;
    float linear;
    float quadratic;
    float lightRange;
};  
#define MAX_NR_POINT_LIGHTS 120
//uniform PointLight uPointLights[MAX_NR_POINT_LIGHTS];
//uniform int uCurrentNrPointLights;
uniform vec3 uViewPos;
uniform Material material;
uniform vec3 skyColor;
uniform vec3 sunDirection;
uniform int drawMode;
uniform int currentAmount;

layout(std430, binding = 1) buffer Lights
{
    PointLight pointLights[];
};

void main()
{
    vec3 viewDir = normalize(uViewPos - fragPos);
    vec3 normal = normalize(vertexNormal);
      // Calc directional light.
    vec3 lightDir = normalize(sunDirection);
    // diffuse shading amount
    float diff = max(dot(vertexNormal, -lightDir), 0.0);
    // specular shading, blinn phong.
    vec3 halfwayDir = normalize(-lightDir + viewDir);  
    vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), 16.0) * material.shininess * vec3(1.0);
    // diffuse result.
    vec3 ambient = skyColor * 0.1;
    vec3 diffuse = skyColor * diff;
    

    for (int i = 0; i < currentAmount; i++)
    {
        float distance = length(pointLights[i].position.xyz - fragPos);
        if (distance < pointLights[i].lightRange)
        {
            vec3 lightDir = normalize(pointLights[i].position.xyz - fragPos);
            // diffuse shading amount.
            float diff = max(dot(normal, lightDir), 0.0);
            // specular shading, blinn phong.
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
            // attenuation.
            float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
  			             pointLights[i].quadratic * (distance * distance));    
            // combine results.
            ambient += pointLights[i].ambient.xyz * attenuation;
            diffuse += pointLights[i].diffuse.xyz * attenuation;
            specular += material.shininess * pointLights[i].specular.xyz * spec * attenuation;
        }
    }
    vec3 lightResult = ambient + diffuse + (specular * vec3(texture(material.specular, textureCoord)));

    if (drawMode == 0)
        FragColor = vec4(lightResult, 1.0) * texture(material.diffuse, textureCoord);
    else if (drawMode == 1)
        FragColor = vec4(lightResult, 1.0);
    else if (drawMode == 2)
        FragColor = vec4(normal, 1.0);
    else if (drawMode == 3)
        FragColor = texture(material.diffuse, textureCoord);
}

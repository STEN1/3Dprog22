#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions_4_5_Core>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>

//#include "GL/glew.h" //We use QOpenGLFunctions instead, so no need for Glew (or GLAD)!

//This class is pretty much a copy of the shader class at
//https://github.com/SonarSystems/Modern-OpenGL-Tutorials/blob/master/%5BLIGHTING%5D/%5B8%5D%20Basic%20Lighting/Shader.h
//which is based on stuff from http://learnopengl.com/ and http://open.gl/.

//must inherit from QOpenGLFunctions_4_1_Core, since we use that instead of glfw/glew/glad
class Shader : protected QOpenGLFunctions_4_5_Core
{
public:
    // Constructor generates the shader on the fly
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ///Use the current shader
    void Use( );

    ///Returns the program number for this shader
    GLuint GetProgram() const;

    void SetMat4f(glm::mat4 matrix, const std::string& uniformName);
    void SetVec4f(glm::vec4 v4, const std::string& uniformName);
    void SetVec3f(glm::vec3 v3, const std::string& uniformName);
    void SetFloat(float f, const std::string& uniformName);
    void SetInt(int i, const std::string& uniformName);
private:
    ///The int OpenGL gives as a reference to this shader
    GLuint m_program;
    std::unordered_map<std::string, uint32_t> m_LocationCache;
};

#endif

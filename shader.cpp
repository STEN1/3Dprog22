#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//#include "GL/glew.h" - using QOpenGLFunctions instead

#include "Core/Logger.h" //For our utility Logger class

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Open files and check for errors
    vShaderFile.open( vertexPath );
    if(!vShaderFile)
        LOG_ERROR("ERROR SHADER FILE " + std::string(vertexPath) + " NOT SUCCESFULLY READ");
    fShaderFile.open( fragmentPath );
    if(!fShaderFile)
        LOG_ERROR("ERROR SHADER FILE " + std::string(fragmentPath) + " NOT SUCCESFULLY READ");
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf( );
    fShaderStream << fShaderFile.rdbuf( );
    // close file handlers
    vShaderFile.close( );
    fShaderFile.close( );
    // Convert stream into string
    vertexCode = vShaderStream.str( );
    fragmentCode = fShaderStream.str( );

    const GLchar *vShaderCode = vertexCode.c_str( );
    const GLchar *fShaderCode = fragmentCode.c_str( );

    // 2. Compile shaders
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    // Print compile errors if any
    glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertex, 512, nullptr, infoLog );
        LOG_ERROR("ERROR SHADER VERTEX " + std::string(vertexPath) +
                         " COMPILATION_FAILED\n" + infoLog);
    }
    // Fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    // Print compile errors if any
    glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( fragment, 512, nullptr, infoLog );
        LOG_ERROR("ERROR SHADER VERTEX " + std::string(fragmentPath) +
                         " COMPILATION_FAILED\n" + infoLog);
    }
    // Shader Program linking
    this->m_program = glCreateProgram();
    glAttachShader( this->m_program, vertex );
    glAttachShader( this->m_program, fragment );
    glLinkProgram( this->m_program );
    // Print linking errors if any
    glGetProgramiv( this->m_program, GL_LINK_STATUS, &success );

    //making nice output name for printing:
    std::string shadername{vertexPath};
    shadername.resize(shadername.size()-5); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog( this->m_program, 512, nullptr, infoLog );
        LOG_ERROR("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" +
                          shadername +  "\n   " + infoLog);
    }
    else
    {
       LOG("GLSL shader " + shadername + " was successfully compiled");
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader( vertex );
    glDeleteShader( fragment );
}

void Shader::Use()
{
    glUseProgram( this->m_program );
}

GLuint Shader::GetProgram() const
{
    return m_program;
}

void Shader::SetMat4f(glm::mat4 matrix, const std::string& uniformName)
{
    uint32_t location;

    if (auto it = m_LocationCache.find(uniformName); it != m_LocationCache.end())
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation(m_program, uniformName.c_str());
        m_LocationCache[uniformName] = location;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetVec4f(glm::vec4 v4, const std::string& uniformName)
{
    uint32_t location;

    if (auto it = m_LocationCache.find(uniformName); it != m_LocationCache.end())
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation(m_program, uniformName.c_str());
        m_LocationCache[uniformName] = location;
    }

    glUniform4f(location, v4.x, v4.y, v4.z, v4.w);
}

void Shader::SetVec3f(glm::vec3 v3, const std::string& uniformName)
{
    uint32_t location;

    if (auto it = m_LocationCache.find(uniformName); it != m_LocationCache.end())
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation(m_program, uniformName.c_str());
        m_LocationCache[uniformName] = location;
    }

    glUniform3f(location, v3.x, v3.y, v3.z);
}

void Shader::SetFloat(float f, const std::string& uniformName)
{
    uint32_t location;

    if (auto it = m_LocationCache.find(uniformName); it != m_LocationCache.end())
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation(m_program, uniformName.c_str());
        m_LocationCache[uniformName] = location;
    }

    glUniform1f(location, f);
}

void Shader::SetInt(int i, const std::string& uniformName)
{
    uint32_t location;

    if (auto it = m_LocationCache.find(uniformName); it != m_LocationCache.end())
    {
        location = it->second;
    }
    else
    {
        location = glGetUniformLocation(m_program, uniformName.c_str());
        m_LocationCache[uniformName] = location;
    }

    glUniform1i(location, i);
}

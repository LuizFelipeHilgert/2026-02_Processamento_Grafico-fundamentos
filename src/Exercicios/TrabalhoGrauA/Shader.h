#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>

class Shader
{
public:

    GLuint ID;

    Shader(
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    void use();
};

#endif
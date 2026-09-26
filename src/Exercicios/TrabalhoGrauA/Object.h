#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Shader.h"

class Object
{
public:

    glm::vec2 position;

    glm::vec2 size;

    GLuint texture;

    Object();

    Object(
        glm::vec2 position,
        glm::vec2 size,
        GLuint texture
    );

    void draw(
        Shader& shader,
        GLuint VAO,
        const glm::mat4& projection
    );
};

#endif
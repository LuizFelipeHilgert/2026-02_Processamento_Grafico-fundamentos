#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Object::Object()
{
    position =
        glm::vec2(
            0.0f,
            0.0f
        );

    size =
        glm::vec2(
            0.0f,
            0.0f
        );

    texture = 0;
}

Object::Object(
    glm::vec2 position,
    glm::vec2 size,
    GLuint texture
)
{
    this->position =
        position;

    this->size =
        size;

    this->texture =
        texture;
}

void Object::draw(
    Shader& shader,
    GLuint VAO,
    const glm::mat4& projection
)
{
    shader.use();


    glm::mat4 model =
        glm::mat4(1.0f);

    model =
        glm::translate(
            model,
            glm::vec3(
                position,
                0.0f
            )
        );

    model =
        glm::scale(
            model,
            glm::vec3(
                size,
                1.0f
            )
        );


    GLint modelLoc =
        glGetUniformLocation(
            shader.ID,
            "model"
        );

    GLint projectionLoc =
        glGetUniformLocation(
            shader.ID,
            "projection"
        );

    GLint useTextureLoc =
        glGetUniformLocation(
            shader.ID,
            "useTexture"
        );

    GLint textureLoc =
        glGetUniformLocation(
            shader.ID,
            "texture1"
        );

    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniformMatrix4fv(
        projectionLoc,
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform1i(
        useTextureLoc,
        true
    );


    glActiveTexture(
        GL_TEXTURE0
    );

    glBindTexture(
        GL_TEXTURE_2D,
        texture
    );

    glUniform1i(
        textureLoc,
        0
    );


    glBindVertexArray(
        VAO
    );

    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );

    glBindVertexArray(0);
}
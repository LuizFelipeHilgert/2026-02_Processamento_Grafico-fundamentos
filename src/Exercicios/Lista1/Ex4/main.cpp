#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int setupShader();
int setupGeometry();

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar *vertexShaderSource = R"glsl(
 #version 410 core
 layout (location = 0) in vec3 position;

 void main()
 {
     gl_Position = vec4(position.x, position.y, position.z, 1.0);
 }
)glsl";

const GLchar *fragmentShaderSource = R"glsl(
 #version 410 core
 uniform vec4 inputColor;
 out vec4 color;

 void main()
 {
     color = inputColor;
 }
)glsl";

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Casa", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);

    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();

    GLuint VAO = setupGeometry();

    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

    glUseProgram(shaderID);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // corpo
        glUniform4f(colorLoc, 0.1f, 0.4f, 0.2f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // telhado
        glUniform4f(colorLoc, 0.1f, 0.1f, 1.1f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 4, 3);

        // porta
        glUniform4f(colorLoc, 0.4f, 0.2f, 0.1f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 7, 4);

        // janela esquerda
        glUniform4f(colorLoc, 0.2f, 0.5f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 11, 4);

        // janela direita
        glDrawArrays(GL_TRIANGLE_STRIP, 15, 4);

        // chaminé
        glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 19, 4);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int setupGeometry()
{
    GLfloat vertices[] = {

        // corpo da casa
        -0.7f, -0.7f, 0.0f,
         0.7f, -0.7f, 0.0f,
        -0.7f,  0.3f, 0.0f,
         0.7f,  0.3f, 0.0f,

        // telhado
        -0.8f, 0.3f, 0.0f,
         0.0f,  1.1f, 0.0f,
         0.8f, 0.3f, 0.0f,

        // porta
        -0.2f, -0.7f, 0.0f,
         0.2f, -0.7f, 0.0f,
        -0.2f,  0.0f, 0.0f,
         0.2f,  0.0f, 0.0f,

        // janela esquerda
        -0.5f, -0.1f, 0.0f,
        -0.3f,  -0.1f, 0.0f,
        -0.5f,  0.1f, 0.0f,
        -0.3f,   0.1f, 0.0f,

        // chaminé
         0.40f, 0.3f, 0.0f,
         0.60f, 0.3f, 0.0f,
         0.40f, 0.60f, 0.0f,
         0.60f, 0.60f, 0.0f
    };

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid *)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <KHR/khrplatform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

// ============================================================
// CONFIGURAÇÕES
// ============================================================

const int WIDTH = 800;
const int HEIGHT = 600;

// Pista
const float ROAD_LEFT = 150.0f;
const float ROAD_RIGHT = 650.0f;
const float ROAD_WIDTH = ROAD_RIGHT - ROAD_LEFT;

// Carro
const float PLAYER_SPEED = 300.0f;

glm::vec2 playerPosition(400.0f, 500.0f);
glm::vec2 playerSize(90.0f, 120.0f);

// Obstáculo
glm::vec2 obstaclePosition(300.0f, -100.0f);
glm::vec2 obstacleSize(65.0f, 90.0f);
glm::vec2 obstacleHitboxSize(30.0f, 50.0f); //ficou muito grande

 float OBSTACLE_SPEED = 300.0f;

bool gameOver = false;

// Movimento da pista
float roadOffset = 0.0f;
const float ROAD_SPEED = 250.0f;


// ============================================================
// LER ARQUIVO
// ============================================================

string readFile(const string& path)
{
    ifstream file(path);

    if (!file.is_open())
    {
        cerr << "Erro ao abrir arquivo: "
             << path
             << endl;

        return "";
    }

    stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}


// ============================================================
// SHADER
// ============================================================

GLuint setupShader()
{
    string vertexSource =
        readFile("shaders/vertex.glsl");

    string fragmentSource =
        readFile("shaders/fragment.glsl");

    if (
        vertexSource.empty() ||
        fragmentSource.empty()
    )
    {
        return 0;
    }

    const char* vertexCode =
        vertexSource.c_str();

    const char* fragmentCode =
        fragmentSource.c_str();


    // --------------------------------------------------------
    // VERTEX SHADER
    // --------------------------------------------------------

    GLuint vertexShader =
        glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexCode,
        nullptr
    );

    glCompileShader(vertexShader);

    int success;

    char infoLog[512];


    glGetShaderiv(
        vertexShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        glGetShaderInfoLog(
            vertexShader,
            512,
            nullptr,
            infoLog
        );

        cerr << "Erro no Vertex Shader:"
             << endl;

        cerr << infoLog
             << endl;

        glDeleteShader(vertexShader);

        return 0;
    }


    // --------------------------------------------------------
    // FRAGMENT SHADER
    // --------------------------------------------------------

    GLuint fragmentShader =
        glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(
        fragmentShader,
        1,
        &fragmentCode,
        nullptr
    );

    glCompileShader(fragmentShader);


    glGetShaderiv(
        fragmentShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        glGetShaderInfoLog(
            fragmentShader,
            512,
            nullptr,
            infoLog
        );

        cerr << "Erro no Fragment Shader:"
             << endl;

        cerr << infoLog
             << endl;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return 0;
    }


    // --------------------------------------------------------
    // LINK
    // --------------------------------------------------------

    GLuint shaderProgram =
        glCreateProgram();

    glAttachShader(
        shaderProgram,
        vertexShader
    );

    glAttachShader(
        shaderProgram,
        fragmentShader
    );

    glLinkProgram(shaderProgram);


    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        glGetProgramInfoLog(
            shaderProgram,
            512,
            nullptr,
            infoLog
        );

        cerr << "Erro ao linkar Shader:"
             << endl;

        cerr << infoLog
             << endl;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        glDeleteProgram(shaderProgram);

        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


// ============================================================
// CARREGAR TEXTURA
// ============================================================

GLuint loadTexture(const string& path)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data =
        stbi_load(
            path.c_str(),
            &width,
            &height,
            &channels,
            0
        );

    if (!data)
    {
        cerr << "Erro ao carregar textura: "
             << path
             << endl;

        cerr << "Motivo: "
             << stbi_failure_reason()
             << endl;

        return 0;
    }

    cout << "Textura carregada: "
         << path
         << " | "
         << width
         << "x"
         << height
         << " | canais: "
         << channels
         << endl;


    GLenum format;

    if (channels == 4)
    {
        format = GL_RGBA;
    }
    else
    {
        format = GL_RGB;
    }


    GLuint texture;

    glGenTextures(
        1,
        &texture
    );

    glBindTexture(
        GL_TEXTURE_2D,
        texture
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );


    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(
        GL_TEXTURE_2D
    );


    stbi_image_free(data);

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );

    return texture;
}


// ============================================================
// GEOMETRIA
// ============================================================

GLuint setupGeometry()
{
    float vertices[] =
    {
        // posição              // textura

        -0.5f, -0.5f, 0.0f,
         0.0f,  0.0f,

         0.5f, -0.5f, 0.0f,
         1.0f,  0.0f,

         0.5f,  0.5f, 0.0f,
         1.0f,  1.0f,


        -0.5f, -0.5f, 0.0f,
         0.0f,  0.0f,

         0.5f,  0.5f, 0.0f,
         1.0f,  1.0f,

        -0.5f,  0.5f, 0.0f,
         0.0f,  1.0f
    };


    GLuint VAO;
    GLuint VBO;


    glGenVertexArrays(
        1,
        &VAO
    );

    glGenBuffers(
        1,
        &VBO
    );


    glBindVertexArray(VAO);

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // --------------------------------------------------------
    // POSIÇÃO
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // --------------------------------------------------------
    // COORDENADA DA TEXTURA
    // --------------------------------------------------------

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );

    glBindVertexArray(0);


    return VAO;
}


// ============================================================
// DESENHAR RETÂNGULO COLORIDO
// ============================================================

void drawRectangle(
    GLuint shader,
    GLuint VAO,
    glm::vec2 position,
    glm::vec2 size,
    glm::vec4 color,
    const glm::mat4& projection
)
{
    glUseProgram(shader);


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
            shader,
            "model"
        );


    GLint projectionLoc =
        glGetUniformLocation(
            shader,
            "projection"
        );


    GLint colorLoc =
        glGetUniformLocation(
            shader,
            "inputColor"
        );


    GLint useTextureLoc =
        glGetUniformLocation(
            shader,
            "useTexture"
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


    glUniform4fv(
        colorLoc,
        1,
        glm::value_ptr(color)
    );


    glUniform1i(
        useTextureLoc,
        false
    );


    glBindVertexArray(VAO);


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    glBindVertexArray(0);
}


// ============================================================
// DESENHAR TEXTURA
// ============================================================

void drawTexture(
    GLuint shader,
    GLuint VAO,
    GLuint texture,
    glm::vec2 position,
    glm::vec2 size,
    const glm::mat4& projection
)
{
    glUseProgram(shader);


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
            shader,
            "model"
        );


    GLint projectionLoc =
        glGetUniformLocation(
            shader,
            "projection"
        );


    GLint useTextureLoc =
        glGetUniformLocation(
            shader,
            "useTexture"
        );


    GLint textureLoc =
        glGetUniformLocation(
            shader,
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


    glActiveTexture(GL_TEXTURE0);


    glBindTexture(
        GL_TEXTURE_2D,
        texture
    );


    glUniform1i(
        textureLoc,
        0
    );


    glBindVertexArray(VAO);


    glDrawArrays(
        GL_TRIANGLES,
        0,
        6
    );


    glBindVertexArray(0);
}


// ============================================================
// MOVIMENTO DO CARRO
// ============================================================

void updatePlayer(
    GLFWwindow* window,
    float deltaTime
)
{
    if (
        glfwGetKey(
            window,
            GLFW_KEY_LEFT
        ) == GLFW_PRESS
    )
    {
        playerPosition.x -=
            PLAYER_SPEED * deltaTime;
    }


    if (
        glfwGetKey(
            window,
            GLFW_KEY_RIGHT
        ) == GLFW_PRESS
    )
    {
        playerPosition.x +=
            PLAYER_SPEED * deltaTime;
    }


    // --------------------------------------------------------
    // LIMITES DA PISTA
    // --------------------------------------------------------

    float halfWidth =
        playerSize.x / 2.0f;


    if (
        playerPosition.x - halfWidth
        <
        ROAD_LEFT
    )
    {
        playerPosition.x =
            ROAD_LEFT + halfWidth;
    }


    if (
        playerPosition.x + halfWidth
        >
        ROAD_RIGHT
    )
    {
        playerPosition.x =
            ROAD_RIGHT - halfWidth;
    }
}


// ============================================================
// COLISÃO
// ============================================================

bool checkCollision(
    glm::vec2 posA,
    glm::vec2 sizeA,
    glm::vec2 posB,
    glm::vec2 sizeB
)
{
    float leftA =
        posA.x - sizeA.x / 2.0f;

    float rightA =
        posA.x + sizeA.x / 2.0f;

    float topA =
        posA.y - sizeA.y / 2.0f;

    float bottomA =
        posA.y + sizeA.y / 2.0f;


    float leftB =
        posB.x - sizeB.x / 2.0f;

    float rightB =
        posB.x + sizeB.x / 2.0f;

    float topB =
        posB.y - sizeB.y / 2.0f;

    float bottomB =
        posB.y + sizeB.y / 2.0f;


    return !(
        leftA > rightB ||
        rightA < leftB ||
        topA > bottomB ||
        bottomA < topB
    );
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // GLFW
    // ========================================================

    if (!glfwInit())
    {
        cerr << "Erro ao inicializar GLFW."
             << endl;

        return -1;
    }


    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        4
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        1
    );

    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );


    // ========================================================
    // JANELA
    // ========================================================

    GLFWwindow* window =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Car Game",
            nullptr,
            nullptr
        );


    if (!window)
    {
        cerr << "Erro ao criar janela."
             << endl;

        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(window);


    // ========================================================
    // GLAD
    // ========================================================

    if (
        !gladLoadGLLoader(
            (GLADloadproc)glfwGetProcAddress
        )
    )
    {
        cerr << "Erro ao inicializar GLAD."
             << endl;

        glfwDestroyWindow(window);

        glfwTerminate();

        return -1;
    }


    // ========================================================
    // INFORMAÇÕES
    // ========================================================

    cout << endl;

    cout << "================================="
         << endl;

    cout << "       CAR GAME - OPENGL"
         << endl;

    cout << "================================="
         << endl;


    cout << "Renderer: "
         << glGetString(GL_RENDERER)
         << endl;


    cout << "OpenGL: "
         << glGetString(GL_VERSION)
         << endl;


    cout << "================================="
         << endl;


    // ========================================================
    // VIEWPORT
    // ========================================================

    glViewport(
        0,
        0,
        WIDTH,
        HEIGHT
    );


    // ========================================================
    // BLENDING
    // ========================================================

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );


    // ========================================================
    // SHADER
    // ========================================================

    GLuint shader =
        setupShader();


    if (shader == 0)
    {
        cerr << "Falha ao criar shader."
             << endl;

        glfwDestroyWindow(window);

        glfwTerminate();

        return -1;
    }


    // ========================================================
    // GEOMETRIA
    // ========================================================

    GLuint VAO =
        setupGeometry();


    // ========================================================
    // TEXTURAS
    // ========================================================

    GLuint carTexture =
        loadTexture(
            "textures/car.png"
        );


    GLuint obstacleTexture =
        loadTexture(
            "textures/obstacle.png"
        );


    if (carTexture == 0)
    {
        cerr << "Erro ao carregar carro."
             << endl;

        glDeleteVertexArrays(
            1,
            &VAO
        );

        glDeleteProgram(shader);

        glfwDestroyWindow(window);

        glfwTerminate();

        return -1;
    }


    if (obstacleTexture == 0)
    {
        cerr << "Erro ao carregar obstáculo."
             << endl;

        glDeleteVertexArrays(
            1,
            &VAO
        );

        glDeleteProgram(shader);

        glfwDestroyWindow(window);

        glfwTerminate();

        return -1;
    }


    // ========================================================
    // PROJEÇÃO ORTOGRÁFICA
    // ========================================================

    glm::mat4 projection =
        glm::ortho(
            0.0f,
            (float)WIDTH,
            (float)HEIGHT,
            0.0f
        );


    // ========================================================
    // TEMPO
    // ========================================================

    float lastFrame =
        (float)glfwGetTime();


    // ========================================================
    // GAME LOOP
    // ========================================================

    while (
        !glfwWindowShouldClose(window)
    )
    {
        // ----------------------------------------------------
        // DELTA TIME
        // ----------------------------------------------------

        float currentFrame =
            (float)glfwGetTime();


        float deltaTime =
            currentFrame - lastFrame;


        lastFrame =
            currentFrame;


        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }


        // ----------------------------------------------------
        // ESC
        // ----------------------------------------------------

        if (
            glfwGetKey(
                window,
                GLFW_KEY_ESCAPE
            ) == GLFW_PRESS
        )
        {
            glfwSetWindowShouldClose(
                window,
                true
            );
        }


        // ====================================================
        // JOGO
        // ====================================================

        if (!gameOver)
        {
            // ------------------------------------------------
            // MOVIMENTO DO CARRO
            // ------------------------------------------------

            updatePlayer(
                window,
                deltaTime
            );


            // ------------------------------------------------
            // MOVIMENTO DO OBSTÁCULO
            // ------------------------------------------------

            obstaclePosition.y +=
                OBSTACLE_SPEED * deltaTime;
            OBSTACLE_SPEED += 20.0f * deltaTime; // Aumenta a velocidade do obstáculo ao longo do tempo
            // ------------------------------------------------
            // COLISÃO
            // ------------------------------------------------

            if (
                checkCollision(
                    playerPosition,
                    playerSize,
                    obstaclePosition,
                    obstacleHitboxSize
                )
            )
            {
                gameOver = true;

                glfwSetWindowTitle(
                    window,
                    "GAME OVER - Pressione R"
                );
            }


            // ------------------------------------------------
            // RESET DO OBSTÁCULO
            // ------------------------------------------------

            if (
                obstaclePosition.y
                >
                HEIGHT + obstacleSize.y
            )
            {
                obstaclePosition.y =
                    -obstacleSize.y;


                obstaclePosition.x =
                    ROAD_LEFT
                    +
                    obstacleSize.x / 2.0f
                    +
                    rand() %
                    (int)(
                        ROAD_WIDTH
                        -
                        obstacleSize.x
                    );
            }


            // ------------------------------------------------
            // MOVIMENTO DA PISTA
            // ------------------------------------------------

            roadOffset +=
                ROAD_SPEED * deltaTime;


            if (roadOffset >= 100.0f)
            {
                roadOffset -= 100.0f;
            }
        }


        // ====================================================
        // REINICIAR
        // ====================================================

        if (
            gameOver &&
            glfwGetKey(
                window,
                GLFW_KEY_R
            ) == GLFW_PRESS
        )
        {
            gameOver = false;

            playerPosition =
                glm::vec2(
                    400.0f,
                    500.0f
                );

            obstaclePosition =
                glm::vec2(
                    300.0f,
                    -100.0f
                );

            roadOffset = 0.0f;


            glfwSetWindowTitle(
                window,
                "Car Game"
            );
        }


        // ====================================================
        // LIMPAR TELA
        // ====================================================

        glClearColor(
            0.2f,
            0.5f,
            0.2f,
            1.0f
        );


        glClear(
            GL_COLOR_BUFFER_BIT
        );


        // ====================================================
        // GRAMA
        // ====================================================

        drawRectangle(
            shader,
            VAO,

            glm::vec2(
                WIDTH / 2.0f,
                HEIGHT / 2.0f
            ),

            glm::vec2(
                WIDTH,
                HEIGHT
            ),

            glm::vec4(
                0.15f,
                0.45f,
                0.18f,
                1.0f
            ),

            projection
        );


        // ====================================================
        // ASFALTO
        // ====================================================

        drawRectangle(
            shader,
            VAO,

            glm::vec2(
                WIDTH / 2.0f,
                HEIGHT / 2.0f
            ),

            glm::vec2(
                ROAD_WIDTH,
                HEIGHT
            ),

            glm::vec4(
                0.12f,
                0.12f,
                0.12f,
                1.0f
            ),

            projection
        );


        // ====================================================
        // BORDA ESQUERDA
        // ====================================================

        drawRectangle(
            shader,
            VAO,

            glm::vec2(
                ROAD_LEFT + 5.0f,
                HEIGHT / 2.0f
            ),

            glm::vec2(
                10.0f,
                HEIGHT
            ),

            glm::vec4(
                0.9f,
                0.9f,
                0.9f,
                1.0f
            ),

            projection
        );


        // ====================================================
        // BORDA DIREITA
        // ====================================================

        drawRectangle(
            shader,
            VAO,

            glm::vec2(
                ROAD_RIGHT - 5.0f,
                HEIGHT / 2.0f
            ),

            glm::vec2(
                10.0f,
                HEIGHT
            ),

            glm::vec4(
                0.9f,
                0.9f,
                0.9f,
                1.0f
            ),

            projection
        );


        // ====================================================
        // FAIXA CENTRAL
        // ====================================================

        for (int i = -1; i < 8; i++)
        {
            float y =
                i * 100.0f
                +
                roadOffset;


            drawRectangle(
                shader,
                VAO,

                glm::vec2(
                    WIDTH / 2.0f,
                    y
                ),

                glm::vec2(
                    10.0f,
                    55.0f
                ),

                glm::vec4(
                    1.0f,
                    0.85f,
                    0.1f,
                    1.0f
                ),

                projection
            );
        }


        // ====================================================
        // OBSTÁCULO
        // ====================================================

        drawTexture(
            shader,
            VAO,
            obstacleTexture,

            obstaclePosition,
            obstacleSize,

            projection
        );


        // ====================================================
        // CARRO
        // ====================================================

        drawTexture(
            shader,
            VAO,
            carTexture,

            playerPosition,
            playerSize,

            projection
        );


        // ====================================================
        // FINAL DO FRAME
        // ====================================================

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    // ========================================================
    // LIMPEZA
    // ========================================================

    glDeleteTextures(
        1,
        &carTexture
    );

    glDeleteTextures(
        1,
        &obstacleTexture
    );


    glDeleteVertexArrays(
        1,
        &VAO
    );


    glDeleteProgram(
        shader
    );


    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
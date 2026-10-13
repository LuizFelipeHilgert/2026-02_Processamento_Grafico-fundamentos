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


const int WIDTH = 800;
const int HEIGHT = 600;

// Pista
const float ROAD_LEFT = 150.0f;
const float ROAD_RIGHT = 650.0f;
const float ROAD_WIDTH = ROAD_RIGHT - ROAD_LEFT;

// Carro
const float PLAYER_SPEED = 300.0f;

glm::vec2 playerPosition(400.0f, 500.0f);
glm::vec2 playerSize(110.0f, 145.0f);

// Obstáculo
glm::vec2 obstaclePosition(300.0f, -100.0f);
glm::vec2 obstacleSize(65.0f, 90.0f);
glm::vec2 obstacleHitboxSize(30.0f, 50.0f); // hitbox menor que a imagem

 float OBSTACLE_SPEED = 300.0f;

bool gameOver = false;
int score = 0;

float roadOffset = 0.0f;
const float ROAD_SPEED = 250.0f;

float treesOffset = 0.0f;



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



GLuint loadTexture(const string& path, bool flipVertically = true)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(flipVertically);

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

GLint wrapMode = GL_CLAMP_TO_EDGE;

if (
    path == "textures/grass.png" ||
    path == "textures/road.png"
)
{
    wrapMode = GL_REPEAT;
}

glTexParameteri(
    GL_TEXTURE_2D,
    GL_TEXTURE_WRAP_S,
    wrapMode
);

glTexParameteri(
    GL_TEXTURE_2D,
    GL_TEXTURE_WRAP_T,
    wrapMode
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



GLuint setupGeometry()
{
    float vertices[] =
    {
        // posição              // UV

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



    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);



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


const unsigned char* getBitmapGlyph(char character)
{
    static const unsigned char space[] = { 0, 0, 0, 0, 0, 0, 0 };
    static const unsigned char A[] = { 14, 17, 17, 31, 17, 17, 17 };
    static const unsigned char E[] = { 31, 16, 16, 30, 16, 16, 31 };
    static const unsigned char G[] = { 15, 16, 16, 23, 17, 17, 15 };
    static const unsigned char M[] = { 17, 27, 21, 21, 17, 17, 17 };
    static const unsigned char N[] = { 17, 25, 21, 19, 17, 17, 17 };
    static const unsigned char O[] = { 14, 17, 17, 17, 17, 17, 14 };
    static const unsigned char P[] = { 30, 17, 17, 30, 16, 16, 16 };
    static const unsigned char R[] = { 30, 17, 17, 30, 20, 18, 17 };
    static const unsigned char S[] = { 15, 16, 16, 14, 1, 1, 30 };
    static const unsigned char T[] = { 31, 4, 4, 4, 4, 4, 4 };
    static const unsigned char V[] = { 17, 17, 17, 17, 17, 10, 4 };
    static const unsigned char zero[] = { 14, 17, 19, 21, 25, 17, 14 };
    static const unsigned char one[] = { 4, 12, 4, 4, 4, 4, 14 };
    static const unsigned char two[] = { 14, 17, 1, 2, 4, 8, 31 };
    static const unsigned char three[] = { 30, 1, 1, 14, 1, 1, 30 };
    static const unsigned char four[] = { 2, 6, 10, 18, 31, 2, 2 };
    static const unsigned char five[] = { 31, 16, 16, 30, 1, 1, 30 };
    static const unsigned char six[] = { 14, 16, 16, 30, 17, 17, 14 };
    static const unsigned char seven[] = { 31, 1, 2, 4, 8, 8, 8 };
    static const unsigned char eight[] = { 14, 17, 17, 14, 17, 17, 14 };
    static const unsigned char nine[] = { 14, 17, 17, 15, 1, 1, 14 };

    switch (character)
    {
        case 'A': return A;
        case 'E': return E;
        case 'G': return G;
        case 'M': return M;
        case 'N': return N;
        case 'O': return O;
        case 'P': return P;
        case 'R': return R;
        case 'S': return S;
        case 'T': return T;
        case 'V': return V;
        case '0': return zero;
        case '1': return one;
        case '2': return two;
        case '3': return three;
        case '4': return four;
        case '5': return five;
        case '6': return six;
        case '7': return seven;
        case '8': return eight;
        case '9': return nine;
        default: return space;
    }
}


void drawBitmapText(
    GLuint shader,
    GLuint VAO,
    const string& text,
    glm::vec2 position,
    float pixelSize,
    glm::vec4 color,
    const glm::mat4& projection
)
{
    for (size_t characterIndex = 0; characterIndex < text.size(); characterIndex++)
    {
        const unsigned char* glyph = getBitmapGlyph(text[characterIndex]);

        for (int row = 0; row < 7; row++)
        {
            for (int column = 0; column < 5; column++)
            {
                if ((glyph[row] & (1 << (4 - column))) == 0)
                {
                    continue;
                }

                drawRectangle(
                    shader,
                    VAO,
                    glm::vec2(
                        position.x + (characterIndex * 6 + column + 0.5f) * pixelSize,
                        position.y + (row + 0.5f) * pixelSize
                    ),
                    glm::vec2(pixelSize * 0.9f, pixelSize * 0.9f),
                    color,
                    projection
                );
            }
        }
    }
}



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



int main()
{

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



    glViewport(
        0,
        0,
        WIDTH,
        HEIGHT
    );



    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );



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



    GLuint VAO =
        setupGeometry();



    GLuint carTexture =
        loadTexture(
            "textures/car.png",
            false
        );


    GLuint obstacleTexture =
        loadTexture(
            "textures/obstacle.png"
        );
    GLuint grassTexture =
        loadTexture("textures/grass.png");

    GLuint roadTexture =
        loadTexture("textures/road.png"); 
    GLuint treesTexture =
        loadTexture("textures/trees.png"); 

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



    glm::mat4 projection =
        glm::ortho(
            0.0f,
            (float)WIDTH,
            (float)HEIGHT,
            0.0f
        );



    float lastFrame =
        (float)glfwGetTime();



    while (
        !glfwWindowShouldClose(window)
    )
    {

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



        if (!gameOver)
        {

            updatePlayer(
                window,
                deltaTime
            );



            obstaclePosition.y +=
                OBSTACLE_SPEED * deltaTime;
            OBSTACLE_SPEED += 20.0f * deltaTime; // velocidade aumenta aos poucos

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



            if (
                obstaclePosition.y
                >
                HEIGHT + obstacleSize.y
            )
            {
                score++;

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



            roadOffset +=
                OBSTACLE_SPEED * deltaTime;


            if (roadOffset >= 100.0f)
            {
                roadOffset -= 100.0f;
            }
            treesOffset += OBSTACLE_SPEED * deltaTime;

            if (treesOffset >= 900.0f)
            {
                treesOffset -= 900.0f;
            }

        }


        if (
            gameOver &&
            glfwGetKey(
                window,
                GLFW_KEY_R
            ) == GLFW_PRESS
        )
        {
            gameOver = false;
            score = 0;

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



        glClearColor(
            0.2f,
            0.5f,
            0.2f,
            1.0f
        );


        glClear(
            GL_COLOR_BUFFER_BIT
        );


drawTexture(
    shader,
    VAO,
    grassTexture,
    glm::vec2(
        WIDTH / 2.0f,
        HEIGHT / 2.0f
    ),
    glm::vec2(
        WIDTH,
        HEIGHT
    ),
    projection
);


for (int i = 0; i < 2; i++)
{
    float y = i * 450.0f + treesOffset;

    // Reaparece no topo após sair da tela
    while (y > HEIGHT + 75.0f)
    {
        y -= 900.0f;
    }

    drawTexture(
        shader,
        VAO,
        treesTexture,
        glm::vec2(90.0f, y),
        glm::vec2(45.0f, 90.0f),
        projection
    );

    drawTexture(
        shader,
        VAO,
        treesTexture,
        glm::vec2(710.0f, y),
        glm::vec2(45.0f, 90.0f),
        projection
    );
}
       


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
        0.20f,
        0.20f,
        0.22f,
        1.0f
    ),
    projection
);

drawRectangle(
    shader,
    VAO,
    glm::vec2(ROAD_LEFT, HEIGHT / 2.0f),
    glm::vec2(10.0f, HEIGHT),
    glm::vec4(0.90f, 0.90f, 0.90f, 1.0f),
    projection
);

drawRectangle(
    shader,
    VAO,
    glm::vec2(ROAD_RIGHT, HEIGHT / 2.0f),
    glm::vec2(10.0f, HEIGHT),
    glm::vec4(0.90f, 0.90f, 0.90f, 1.0f),
    projection
);


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



        drawTexture(
            shader,
            VAO,
            obstacleTexture,

            obstaclePosition,
            obstacleSize,

            projection
        );



        drawTexture(
            shader,
            VAO,
            carTexture,

            playerPosition,
            playerSize,

            projection
        );


        stringstream scoreText;
        scoreText << "PONTOS " << score;

        drawBitmapText(
            shader,
            VAO,
            scoreText.str(),
            glm::vec2(20.0f, 20.0f),
            3.0f,
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            projection
        );


        glfwSwapBuffers(window);

        glfwPollEvents();
    }



glDeleteTextures(
    1,
    &carTexture
);

glDeleteTextures(
    1,
    &obstacleTexture
);


glDeleteTextures(
    1,
    &grassTexture
);

glDeleteTextures(
    1,
    &roadTexture
);

glDeleteVertexArrays(
    1,
    &VAO
);

glDeleteProgram(
    shader
);

glDeleteTextures(
    1,
    &treesTexture
);

    return 0;
}

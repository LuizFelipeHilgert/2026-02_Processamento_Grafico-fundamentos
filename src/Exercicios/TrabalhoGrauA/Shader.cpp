#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Shader::Shader(
    const string& vertexPath,
    const string& fragmentPath
)
{

    ifstream vertexFile(
        vertexPath
    );

    ifstream fragmentFile(
        fragmentPath
    );

    if (
        !vertexFile.is_open() ||
        !fragmentFile.is_open()
    )
    {
        cerr
            << "Erro ao abrir arquivo de shader."
            << endl;

        ID = 0;

        return;
    }

    stringstream vertexStream;
    stringstream fragmentStream;

    vertexStream
        << vertexFile.rdbuf();

    fragmentStream
        << fragmentFile.rdbuf();

    string vertexSource =
        vertexStream.str();

    string fragmentSource =
        fragmentStream.str();

    const char* vertexCode =
        vertexSource.c_str();

    const char* fragmentCode =
        fragmentSource.c_str();


    GLuint vertexShader =
        glCreateShader(
            GL_VERTEX_SHADER
        );

    glShaderSource(
        vertexShader,
        1,
        &vertexCode,
        nullptr
    );

    glCompileShader(
        vertexShader
    );

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

        cerr
            << "Erro no Vertex Shader:"
            << endl;

        cerr
            << infoLog
            << endl;

        glDeleteShader(
            vertexShader
        );

        ID = 0;

        return;
    }


    GLuint fragmentShader =
        glCreateShader(
            GL_FRAGMENT_SHADER
        );

    glShaderSource(
        fragmentShader,
        1,
        &fragmentCode,
        nullptr
    );

    glCompileShader(
        fragmentShader
    );

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

        cerr
            << "Erro no Fragment Shader:"
            << endl;

        cerr
            << infoLog
            << endl;

        glDeleteShader(
            vertexShader
        );

        glDeleteShader(
            fragmentShader
        );

        ID = 0;

        return;
    }


    ID =
        glCreateProgram();

    glAttachShader(
        ID,
        vertexShader
    );

    glAttachShader(
        ID,
        fragmentShader
    );

    glLinkProgram(
        ID
    );

    glGetProgramiv(
        ID,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        glGetProgramInfoLog(
            ID,
            512,
            nullptr,
            infoLog
        );

        cerr
            << "Erro ao linkar Shader:"
            << endl;

        cerr
            << infoLog
            << endl;

        glDeleteShader(
            vertexShader
        );

        glDeleteShader(
            fragmentShader
        );

        glDeleteProgram(
            ID
        );

        ID = 0;

        return;
    }

    glDeleteShader(
        vertexShader
    );

    glDeleteShader(
        fragmentShader
    );
}

void Shader::use()
{
    glUseProgram(
        ID
    );
}
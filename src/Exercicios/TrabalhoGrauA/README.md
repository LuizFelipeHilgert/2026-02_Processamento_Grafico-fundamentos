# Jogo de Carro — Trabalho Grau A

# Aluno:
Luiz Felipe Hilgert

## Sobre o jogo

Este é um jogo 2D feito em C++ com OpenGL. O jogador controla um carro em uma pista e precisa desviar de um obstáculo que vem em sua direção. A pontuação aumenta quando o obstáculo passa pelo carro. Se os dois colidirem, o jogo termina.

O cenário contém a pista, grama, árvores e imagens do carro e do obstáculo. O jogo também mostra a pontuação na tela.

## Controles

- **Seta para a esquerda:** move o carro para a esquerda.
- **Seta para a direita:** move o carro para a direita.
- **R:** reinicia o jogo depois do fim.

Quando ocorre uma colisão, o título da janela mostra **GAME OVER — Pressione R**.

## Requisitos

- CMake.
- Um compilador C++17.
- Git, para o CMake baixar GLFW, GLM e stb_image na primeira configuração.
- OpenGL disponível no sistema.

O projeto já inclui os arquivos da GLAD.

## Compilar e executar

Abra o Terminal na pasta principal do repositório, aquela que contém `CMakeLists.txt`, e execute:

```bash
cmake -S . -B build
cmake --build build --target Exercicios_TrabalhoGrauA_main
cd src/Exercicios/TrabalhoGrauA
../../../build/Exercicios_TrabalhoGrauA_main
```

O último comando deve ser executado dentro da pasta `src/Exercicios/TrabalhoGrauA`.



## Compilar e executar no Windows

## Pré-requisitos

CMake instalado.

Compilador C++ compatível, como o disponibilizado pelo Visual Studio Community com a carga de trabalho Desenvolvimento para desktop com C++.

Git instalado.

OpenGL disponível no sistema.

## Compilação

Abra o PowerShell ou o terminal do VS Code na pasta principal do repositório e execute:

cmake -S . -B build
cmake --build build --target Exercicios_TrabalhoGrauA_main

## Execução

Entre na pasta do jogo:

cd src/Exercicios/TrabalhoGrauA

Execute o programa:

..\..\..\build\Exercicios_TrabalhoGrauA_main.exe

Dependendo do gerador do CMake, o executável pode ser criado em uma subpasta, como build\Debug ou build\Release. Nesse caso, ajuste o caminho de execução.



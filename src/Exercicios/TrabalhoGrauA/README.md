# Jogo de Carro — Trabalho Grau A

**Aluno:** Luiz Felipe Hilgert

## Sobre o jogo

Este é um jogo 2D desenvolvido em C++ com OpenGL. O jogador controla um carro em uma pista e precisa desviar de um obstáculo que vem em sua direção. A pontuação aumenta quando o obstáculo passa pelo carro. Se os dois colidirem, o jogo termina.

O cenário contém a pista, grama, árvores e imagens do carro e do obstáculo. O jogo também mostra a pontuação na tela.

## Controles

- **Seta para a esquerda:** move o carro para a esquerda.
- **Seta para a direita:** move o carro para a direita.
- **R:** reinicia o jogo depois do fim.
- **Esc:** fecha o jogo.

Quando ocorre uma colisão, o título da janela mostra `GAME OVER - Pressione R`.

## Requisitos

- CMake.
- Compilador C++17 compatível com o projeto.
- Git, para o CMake baixar as dependências durante a configuração.
- OpenGL disponível no sistema.

O projeto já inclui os arquivos da GLAD.

## Compilar e executar no macOS

Abra o Terminal na pasta principal do repositório, aquela que contém `CMakeLists.txt`, e execute:

```bash
cmake -S . -B build
cmake --build build --target Exercicios_TrabalhoGrauA_main
cd src/Exercicios/TrabalhoGrauA
../../../build/Exercicios_TrabalhoGrauA_main
```

O último comando deve ser executado dentro da pasta `src/Exercicios/TrabalhoGrauA`, pois o jogo utiliza caminhos relativos para carregar os shaders e as texturas.

## Compilar e executar no Windows

### Pré-requisitos

- CMake instalado.
- Compilador C++ compatível, como o disponibilizado pelo Visual Studio Community com a carga de trabalho **Desenvolvimento para desktop com C++**.
- Git instalado.
- OpenGL disponível no sistema.

### Compilação

Abra o PowerShell ou o terminal do VS Code na pasta principal do repositório e execute:

```powershell
cmake -S . -B build
cmake --build build --target Exercicios_TrabalhoGrauA_main
```

### Execução

Entre na pasta do jogo:

```powershell
cd src/Exercicios/TrabalhoGrauA
```

Execute o programa:

```powershell
..\..\..\build\Exercicios_TrabalhoGrauA_main.exe
```

Dependendo do gerador do CMake, o executável pode ser criado em uma subpasta, como `build\Debug` ou `build\Release`. Nesse caso, ajuste o caminho de execução.

O programa deve ser executado a partir da pasta do jogo para encontrar corretamente os arquivos de shaders e texturas.
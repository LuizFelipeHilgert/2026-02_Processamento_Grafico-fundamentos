# Processamento Gráfico — 2026/2

Repositório usado para organizar os exercícios, exemplos e trabalhos da disciplina de Processamento Gráfico: Fundamentos.

Os programas são feitos em C++ com OpenGL. Para montar a janela e acessar as funções gráficas, os códigos usam GLFW e GLAD.

## Organização

```text
PG2026-2/
├── Common/                       # Arquivo comum da GLAD
├── include/                      # Cabeçalhos usados pelos programas
├── src/
│   ├── Exemplos/                 # Códigos de exemplo vistos em aula
│   └── Exercicios/
│       ├── Lista1/               # Exercícios sobre primitivas e formas 2D
│       ├── Lista2/               # Pasta reservada para a segunda lista
│       ├── TrabalhoGrauA/         # Pasta reservada para o trabalho do Grau A
│       └── TrabalhoGrauB/         # Pasta reservada para o trabalho do Grau B
├── misc/                         # Materiais auxiliares e exemplos
├── CMakeLists.txt                # Configuração de compilação do projeto
└── GettingStarted.md             # Orientações para configurar e compilar
```

Cada exercício possui uma pasta própria, com seu código-fonte, um `README.md` explicando a atividade e um `.gitignore` para não enviar arquivos gerados pela compilação.

## Exercícios já incluídos

- **Lista 1:** desenhos de triângulos, polígonos, círculo, estrela, espiral, triângulo colorido e uma casa feita com primitivas do OpenGL.
- **Lista 2:** estrutura de pastas preparada para os próximos exercícios.
- **Trabalhos Grau A e Grau B:** pastas separadas para os trabalhos da disciplina.

## Como compilar e executar

É necessário ter o CMake e um compilador C++ instalados. No terminal, dentro da pasta do repositório, execute:

```bash
cmake -S . -B build
cmake --build build
```

O `CMakeLists.txt` cria um executável para cada arquivo `.cpp` que possui uma função `main()`. Por exemplo, os exercícios `a.cpp`, `b.cpp`, `c.cpp` e `d.cpp` da Lista 1 podem ser compilados separadamente, sem misturar vários `main()` no mesmo programa.

Para abrir todos os programas implementados de uma vez, depois da compilação, execute:

```bash
for programa in \
  build/Exemplos_HelloTriangle_HelloTriangle \
  build/Exercicios_Lista1_Ex1_a \
  build/Exercicios_Lista1_Ex1_b \
  build/Exercicios_Lista1_Ex1_c \
  build/Exercicios_Lista1_Ex1_d \
  build/Exercicios_Lista1_Ex2_a \
  build/Exercicios_Lista1_Ex2_b \
  build/Exercicios_Lista1_Ex2_c \
  build/Exercicios_Lista1_Ex2_circulo \
  build/Exercicios_Lista1_Ex2_d \
  build/Exercicios_Lista1_Ex2_e \
  build/Exercicios_Lista1_Ex2_f \
  build/Exercicios_Lista1_Ex3_main \
  build/Exercicios_Lista1_Ex4_main
do
  "$programa" &
done

wait
```

Os arquivos vazios das atividades que ainda não foram desenvolvidas não geram executáveis.

> Observação: a pasta `build/` é criada pelo CMake e não faz parte dos códigos dos exercícios.

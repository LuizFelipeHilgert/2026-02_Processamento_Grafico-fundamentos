# Lista 1 — Exercício 3

Código do exercício será adicionado aqui.

a) Configuração dos buffers (VBO e VAO)
Para representar o triângulo, utilizei um VBO contendo a posição e a cor de cada vértice. Cada vértice possui 6 valores: 3 para posição (x, y, z) e 3 para cor (r, g, b).
GLfloat vertices[] = {
     0.0f,  0.8f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.8f, -0.7f, 0.0f,   0.0f, 1.0f, 0.0f,
     0.8f, -0.4f, 0.0f,   0.0f, 0.0f, 1.0f
};
O VAO é utilizado para configurar como esses dados serão interpretados. O atributo de posição fica no location 0 e o atributo de cor fica no location 1.
b) Identificação dos atributos no vertex shader
No vertex shader, os atributos são identificados através do layout(location):
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexColor;
A posição é usada para definir onde o vértice será desenhado e a cor é passada para o fragment shader. Dessa forma, o P1 fica vermelho, o P2 verde e o P3 azul.

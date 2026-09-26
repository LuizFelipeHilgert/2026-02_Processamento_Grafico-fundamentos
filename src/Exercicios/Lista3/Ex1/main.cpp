#include <iostream>
#include <string>
#include <assert.h>

#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int setupShader();
int setupGeometry();
void initializeColors();
void addVertex(vec3 pos);
void updateGeometry();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"glsl(
 #version 410 core
 layout (location = 0) in vec3 position;
 uniform mat4 projection;
 uniform mat4 model;
 void main()
 {
	 gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
 }
 )glsl";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"glsl(
 #version 410 core
 uniform vec4 inputColor;
 out vec4 color;
 void main()
 {
	 color = inputColor;
 }
 )glsl";

// Para o exercicio 1 da Lista 3

vector <vec3> vertices;
vector <vec3> colors;

int iColor = 0;

GLuint VBO;
GLuint VAO;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Lista 3 - Triangulos", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	if (shaderID == 0)
	{
		glfwTerminate();
		return -1;
	}

	// Gerando um buffer simples, com a geometria dos vertices
	GLuint geometry = setupGeometry();

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	glUseProgram(shaderID); // Reseta o estado do shader para evitar problemas futuros

	double prev_s = glfwGetTime();	// Define o "tempo anterior" inicial.
	double title_countdown_s = 0.1; // Intervalo para atualizar o título da janela com o FPS.

	// Criação da matriz de projeção - projection matrix
	mat4 projection = ortho(0.0,800.0,0.0,600.0,-1.0,1.0);

	// Para mandar pro shader
	GLint projLoc = glGetUniformLocation(shaderID,"projection");
	glUniformMatrix4fv(projLoc,1,GL_FALSE,value_ptr(projection));

	// Localização da matriz model
	GLint modelLoc = glGetUniformLocation(shaderID,"model");

	// Matriz de transformações do objeto (triângulo) - model matrix
	mat4 model = mat4(1); // matriz identidade

	initializeColors();

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Este trecho de código é totalmente opcional: calcula e mostra a contagem do FPS na barra de título
		{
			double curr_s = glfwGetTime();		// Obtém o tempo atual.
			double elapsed_s = curr_s - prev_s; // Calcula o tempo decorrido desde o último frame.
			prev_s = curr_s;					// Atualiza o "tempo anterior" para o próximo frame.

			// Exibe o FPS, mas não a cada frame, para evitar oscilações excessivas.
			title_countdown_s -= elapsed_s;
			if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
			{
				double fps = 1.0 / elapsed_s; // Calcula o FPS com base no tempo decorrido.

				// Cria uma string e define o FPS como título da janela.
				char tmp[256];
				snprintf(tmp, sizeof(tmp), "Lista 3 - Triangulos\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);

				title_countdown_s = 0.1; // Reinicia o temporizador para atualizar o título periodicamente.
			}
		}

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(10);

		glBindVertexArray(geometry); // Conectando ao buffer de geometria

		glUseProgram(shaderID);

		// Usa a matriz identidade para que os vertices sejam desenhados
		// exatamente nas posições onde o usuário clicou
		glUniformMatrix4fv(modelLoc,1,GL_FALSE,value_ptr(model));

		// Desenha os triangulos já completos
		for (size_t i = 0; i + 2 < vertices.size(); i += 3)
		{
			size_t triangleIndex = i / 3;

			glUniform4f(
				colorLoc,
				colors[triangleIndex].r,
				colors[triangleIndex].g,
				colors[triangleIndex].b,
				1.0f
			);

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES
			glDrawArrays(GL_TRIANGLES, i, 3);
		}

		// Desenha os vertices que ainda não completaram um triangulo
		// Isso permite visualizar cada clique realizado pelo usuário
		size_t completedVertices = (vertices.size() / 3) * 3;

		if (completedVertices < vertices.size())
		{
			glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

			glDrawArrays(
				GL_POINTS,
				completedVertices,
				vertices.size() - completedVertices
			);
		}

		// glBindVertexArray(0); // Desnecessário aqui, pois não há múltiplos VAOs

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &geometry);
	glDeleteBuffers(1, &VBO);

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;

		glDeleteShader(vertexShader);
		return 0;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return 0;
	}

	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);

		return 0;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int setupGeometry()
{
	GLfloat verticesInicial[] = {
		// x   y     z
		0.0f, 0.0f, 0.0f
	};

	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Cria o buffer inicialmente vazio
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
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

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		// Converte a coordenada Y da janela para a coordenada do mundo
		// considerando a origem no canto inferior esquerdo
		vec3 position(xpos, HEIGHT - ypos, 0.0f);

		addVertex(position);
	}
}

void initializeColors()
{
	colors.push_back(vec3(1.0,0.0,0.0));
	colors.push_back(vec3(0.0,1.0,0.0));
	colors.push_back(vec3(0.0,0.0,1.0));
	colors.push_back(vec3(1.0,1.0,0.0));
	colors.push_back(vec3(1.0,0.0,1.0));
	colors.push_back(vec3(0.0,1.0,1.0));
}

void addVertex(vec3 pos)
{
	// Adiciona apenas um vertice a cada clique
	vertices.push_back(pos);

	// A cada 3 vertices criados, um novo triangulo é formado
	if (vertices.size() % 3 == 0)
	{
		iColor = (iColor + 1) % colors.size();
	}

	// Atualiza o buffer de geometria com os vertices criados
	updateGeometry();
}

void updateGeometry()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vec3),
		vertices.data(),
		GL_DYNAMIC_DRAW
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
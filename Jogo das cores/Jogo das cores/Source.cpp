
#include <iostream>
#include <string>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

using namespace std;

//Classe para manipulação dos shaders
#include "Shader.h"

// Protótipo da função de callback da GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1024, HEIGHT = 768;
const int COLUMNS = 5, LINES = 11;
const float SIMILARITY_COLORS = 0.05f;
int colorsInScreen = 0;
int points = 100;

// Protótipos das funções
int setup();
void pickColor(GLdouble xpos, GLdouble ypos);
void initRandomColors(glm::vec3 colorMatrix[COLUMNS][LINES]);

glm::vec3 colorMatrix[COLUMNS][LINES];

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das cores", nullptr, nullptr);
	glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o shader
	Shader shader("../shaders/retangulo.vs", "../shaders/retangulo.fs");
	Shader shaderPalete("../shaders/palete.vs", "../shaders/palete.fs");

	// Gerando um buffer simples
	GLuint VAO = setup();
	
	glm::mat4 model = glm::mat4(1);

	initRandomColors(colorMatrix);
	bool firstLoad = true;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{

		if (colorsInScreen <= 0) {
			if (!firstLoad)
				cout << "Sua Pontuacao foi: " << points << "%" << endl;
			firstLoad = false;
			points = 100;
			initRandomColors(colorMatrix);
			colorsInScreen = COLUMNS * LINES;
		}
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO); //Conectando ao buffer de geometria
		
		shader.Use();
		GLfloat xc = -0.77f,
			    xl = 0.90f;
		for (int c = 0; c < COLUMNS; c++) 
			for (int l = 0; l < LINES; l++) {

			shader.setVec3("cor", colorMatrix[c][l].r, colorMatrix[c][l].g, colorMatrix[c][l].b); // sugestão colorMatrix[COLUMNS][LINES] 

			model = glm::mat4(1);

			model = glm::translate(model, glm::vec3(xc + c * 0.385, xl - l * 0.125, 0)); // sugestão
			model = glm::scale(model, glm::vec3(0.38, 0.38, 1)); // sugestão

			shader.setMat4("model", glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		shaderPalete.Use();

		model = glm::mat4(1);

		model = glm::translate(model, glm::vec3(0.70, -0.70, 0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 1));

		shaderPalete.setMat4("model", glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

		glBindVertexArray(0); //Desconectando o buffer de geometria

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Função de callback de clique do botão
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	GLdouble xpos, ypos;
	int w, h;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwGetWindowSize(window, &w, &h);
		pickColor(xpos, h - ypos); // cor do pixel
	}
}

// Esta função está bastante harcoded 
int setup()
{
	GLfloat vertices[] = {
		-0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,  // Vértice retângulo
		-0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
		0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
		0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,    // Vértice retângulo 
		-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Vértice cores  (R)
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Vértice cores  (G)
		0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // Vértice cores  (B)
	   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f   // Vértice cores  (RGB)
	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Atributo 0 - posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo 1 - cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void pickColor(GLdouble xpos, GLdouble ypos) {
	float pixel[4];
	int miss = 0;

	glReadPixels(xpos, ypos, 1, 1, GL_RGBA, GL_FLOAT, &pixel);

	if (pixel[0] == 0.0f && pixel[1] == 0.0f && pixel[2] == 0.0f)
		return;

	for (int c = 0; c < COLUMNS; ++c) {
		for (int l = 0; l < LINES; ++l) {
			
			if (colorMatrix[c][l].r == 0.0f && colorMatrix[c][l].g == 0.0f && colorMatrix[c][l].b == 0.0f)
				continue;

			if ((0.3 * ((colorMatrix[c][l].r - pixel[0]) * (colorMatrix[c][l].r - pixel[0]))) +
				(0.59 * ((colorMatrix[c][l].g - pixel[1]) * (colorMatrix[c][l].g - pixel[1]))) +
				(0.11 * ((colorMatrix[c][l].b - pixel[2]) * (colorMatrix[c][l].b - pixel[1]))) < SIMILARITY_COLORS){
				colorMatrix[c][l] = glm::vec3(0.0f, 0.0f, 0.0f);
				colorsInScreen--;
			}
			else
				miss++;

		}
	}
	points = points - points * ((float)miss / ((float)COLUMNS*(float)LINES));
}

void initRandomColors(glm::vec3 colorMatrix[COLUMNS][LINES]) {
	// Random number generation setup
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	for (int c = 0; c < COLUMNS; ++c) {
		for (int l = 0; l < LINES; ++l) {
			// Generate random colors
			float r = dis(gen);
			float g = dis(gen);
			float b = dis(gen);
			colorMatrix[c][l] = glm::vec3(r, g, b);
		}
	}
}
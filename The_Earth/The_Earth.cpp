#include <iostream>
#include <locale.h>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;
using namespace glm;

const int largura = 1280;
const int altura = 720;

int main()
{
	setlocale(LC_ALL, "Portuguese");
	// Inicia a GLFW e verifica caso ocorra algum erro.
	if (glfwInit() == GLFW_FALSE){

		cout << "Erro ao iniciar o GLFW";
	}

	// Criação da janela principal.
	GLFWwindow* janela = glfwCreateWindow(largura, altura, "The Earth", nullptr, nullptr);

	glfwMakeContextCurrent(janela);

	// Inicializa o GLEW
	if (glewInit() != GLEW_OK) {
		cout << "Erro ao iniciar o GLEW";
	}

	//Verifica a versão do OpenGL e obtém algumas informações acerca do driver de vídeo.
	GLint GLVersaoMaior = 0;
	GLint GLVersaoMenor = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &GLVersaoMaior);
	glGetIntegerv(GL_MINOR_VERSION, &GLVersaoMenor);

	cout << "Fabricante da GPU: " << glGetString(GL_VENDOR) << "\n";
	cout << "Renderizador OpenGL: " << glGetString(GL_RENDERER) << "\n";
	cout << "Versão do OpenGL: " << glGetString(GL_VERSION) << "\n";
	cout << "Versão do GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

	//Definição do triango usando coordenadas normalizadas
	array < glm::vec3, 3> Triangulo = {
		glm::vec3{-1.0f, -1.0f, 0.0f},
		glm::vec3{ 1.0f, -1.0f, 0.0f}, 
		glm::vec3{ 0.0f,  1.0f, 0.0f}
	};

	//Matriz Modelo
	mat4 MatrizModelo = identity<mat4>();

	//Matriz de Visão
	vec3 Olho{ 0, 0, 10};
	vec3 Centro{0, 0, 0};
	vec3 Cima{ 0, 1, 0 };
	mat4 MatrizVisao = lookAt(Olho, Centro, Cima);

	//Matriz de Projeção
	constexpr float FoV = radians(45.0f);
	const float AspectRatio = largura / altura;
	const float Near = 0.001f;
	const float Far = 1000.0f;
	mat4 MatrizProjecao = perspective(FoV, AspectRatio, Near, Far);

	// Model View Projection - MVP
	mat4 MVP = MatrizProjecao * MatrizVisao * MatrizModelo;

	//Aplicando a MVP nos vertices do triangulo.
	for (vec3& Vertice : Triangulo) {
		vec4 VerticeProjetado = MVP * vec4{ Vertice, 1.0f };
		VerticeProjetado /= VerticeProjetado.w;
		Vertice = VerticeProjetado;
	}

	//Copiando os vértices do triangulo para a memória da GPU
	GLuint Buffer_Vertices;

	//Gera o identificador do buffer de vertices
	glGenBuffers(1, &Buffer_Vertices);

	//Ativa o buffer de vertices, onde serão copiados os dados do triangulo.
	glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);

	//Copiar os dados para a memoria de video
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangulo), Triangulo.data(), GL_STATIC_DRAW);

	//Define a cor de fundo
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	// Loop de eventos da aplicação.
	while (!glfwWindowShouldClose(janela)) {

		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(0);

		//informa o opengl que o Buffer de Vertices vai ser o ativo no momento.
		glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Reverter o estado que foi criado
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);

		// Realiza os processamentos em fila do GLFW, e esses eventos podem consistir de teclas apertadas, interação com o mouse etc.
		glfwPollEvents();

		// Envia para ser desenhado na tela o conteúdo do framebuffer.
		glfwSwapBuffers(janela);
	}

	//Desaloca o Buffer de Vertices
	glDeleteBuffers(1, &Buffer_Vertices);

	// Comando para o encerramento da biblioteca GLFW
	glfwTerminate();


	return 0;

}
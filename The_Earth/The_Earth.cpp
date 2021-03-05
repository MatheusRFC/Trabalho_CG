#include <iostream>
#include <locale.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

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


	// Loop de eventos da aplicação.
	while (!glfwWindowShouldClose(janela)) {

		// Realiza os processamentos em fila do GLFW, e esses eventos podem consistir de teclas apertadas, interação com o mouse etc.
		glfwPollEvents();

		// Envia para ser desenhado na tela o conteúdo do framebuffer.
		glfwSwapBuffers(janela);
	}

	// Comando para o encerramento da biblioteca GLFW
	glfwTerminate();


	return 0;

}
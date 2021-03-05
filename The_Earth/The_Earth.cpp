#include <iostream>

#include <GLFW/glfw3.h>

using namespace std;

const int largura = 1280;
const int altura = 720;

int main()
{

	// Inicia a GLFW e verifica caso ocorra algum erro.
	if (glfwInit() == GLFW_FALSE){

		cout << "Erro ao iniciar o GLFW";
	}

	// Criação da janela principal.
	GLFWwindow* janela = glfwCreateWindow(largura, altura, "The Earth", nullptr, nullptr);

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
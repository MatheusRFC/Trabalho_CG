﻿#include <iostream>
#include <locale.h>
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <assert.h>
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace std;
using namespace glm;

const int largura = 1280;
const int altura = 720;

string LerArquivo(const char* caminho) 
{
	string Conteudo_arquivo;
	if (ifstream arquivo{ caminho, ios::in })
	{
		Conteudo_arquivo.assign(istreambuf_iterator<char>(arquivo), istreambuf_iterator<char>());
	}
	return Conteudo_arquivo;
}


//Função que verifica o shader. PS: ShaderID já deve estar compilado antes de ser passado pra função.
void verificaShader(GLuint ShaderId)
{
	GLint resultado = GL_TRUE;
	glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &resultado);

	//Verifica se houve algum erro na hora da compilação do shader.
	if (resultado == GL_FALSE)
	{
		GLint Comprimento_log = 0;
		glGetShaderiv(ShaderId, GL_INFO_LOG_LENGTH, &Comprimento_log);

		if (Comprimento_log > 0) {
			string Info_Log_Shader(Comprimento_log, '\0');
			glGetShaderInfoLog(ShaderId, Comprimento_log, nullptr, &Info_Log_Shader[0]);

			cout << "Erro no Shader" << endl;
			cout << Info_Log_Shader << endl;

			assert(false);
		}
	}
}

GLuint CarregaShaders(const char* VertexShaderFile, const char* FragmentShaderFile) 
{
	string FonteVertexShader = LerArquivo(VertexShaderFile);
	string FonteFragmentShader = LerArquivo(FragmentShaderFile);

	assert(!FonteVertexShader.empty());
	assert(!FonteFragmentShader.empty());

	//Criação dos identificadores dos arquivos.
	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	//Verifica se a compilação do vertexshader deu certo.
	cout << "Compilando " << VertexShaderFile << endl;
	const char* FonteVertexShaderPtr = FonteVertexShader.c_str();
	glShaderSource(VertexShaderId, 1, &FonteVertexShaderPtr, nullptr);
	glCompileShader(VertexShaderId);
	verificaShader(VertexShaderId);

	//Verifica se a compilação do fragmentshader deu certo.
	cout << "Compilando " << FragmentShaderFile << endl;
	const char* FonteFragmentShaderPtr = FonteFragmentShader.c_str();
	glShaderSource(FragmentShaderId, 1, &FonteFragmentShaderPtr, nullptr);
	glCompileShader(FragmentShaderId);
	verificaShader(FragmentShaderId);

	cout << "Linkando o programa" << endl;
	GLuint ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);

	//Verifica a linkagem do programa
	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE)
	{

		GLint Tamanho_log = 0;
		glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &Tamanho_log);

		if (Tamanho_log > 0) 
		{
			string info_log(Tamanho_log, '\0');
			glGetProgramInfoLog(ProgramId, Tamanho_log, nullptr, &info_log[0]);

			cout << "Erro na linkagem do programa" << endl;
			cout << info_log << endl;
		}

		assert(false);
	}

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	return ProgramId;
}

GLuint CarregaTextura(const char* Arquivo_Textura)
{
	cout << "Carregando textura: " << Arquivo_Textura << endl;

	stbi_set_flip_vertically_on_load(true);

	int Tamanho_textura = 0;
	int Altura_textura = 0;
	int Numero_compomentes = 0;
	unsigned char* Dados_textura = stbi_load(Arquivo_Textura, &Tamanho_textura, &Altura_textura, &Numero_compomentes, 3);

	assert(Dados_textura);

	//Gera o identificador da textura
	GLuint TextureId;
	glGenTextures(1, &TextureId);

	//Permite modificar a textura
	glBindTexture(GL_TEXTURE_2D, TextureId);

	//Copia a textura para a memoria de vídeo
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Tamanho_textura, Altura_textura, 0, GL_RGB, GL_UNSIGNED_BYTE, Dados_textura);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Configura o texture mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Gera o MIPMAP a partir da textura.
	glGenerateMipmap(GL_TEXTURE_2D);

	//Desabilita a textura depois de ela ser copiada para a GPU.
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(Dados_textura);

	return TextureId;
}

struct Vertex
{
	vec3 Posicao;
	vec3 Cor;
	vec2 UV;
};

class FlyCamera
{
public:

	void emFrente(float valor)
	{
		Localizacao += Direcao * valor * velocidade;
	}

	void aDireita(float valor)
	{
		vec3 Direita = normalize(cross(Direcao, Cima));
		Localizacao += Direita * valor * velocidade;
	}

	mat4 GetViewProjection() const
	{
		mat4 View = lookAt(Localizacao, Localizacao + Direcao, Cima);
		mat4 Projection = perspective(FieldOfView, AspectRatio, Near, Far);
		return Projection * View;
	}

	//Parametros de interatividade
	float velocidade = 20.0f;


	//Definição da matriz de visão.
	vec3 Localizacao{ 0.0f, 0.0f, 10.0f };
	vec3 Direcao{0.0f, 0.0f, -1.0f};
	vec3 Cima{ 0.0f, 1.0f, 0.0f };

	//Definição da matriz de projeção
	float FieldOfView = radians(45.0f);
	float AspectRatio = largura / altura;
	float Near = 0.01f;
	//OBS: Mudar o plano far para adicionar mais planetas.
	float Far = 1000.0f;

};

FlyCamera Camera;
bool HabilitaMovimentoMouse = false;
vec2 CursorAnterior{ 0.0, 0.0 };

void ClickMouse(GLFWwindow* janela, int botao, int acao, int modificadores)
{
	cout << "Botao: " << botao << " Acao: " << acao << " Modificadores: " << modificadores << endl;

	if (botao == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (acao == GLFW_PRESS)
		{
			glfwSetInputMode(janela, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double x, y;
			glfwGetCursorPos(janela, &x, &y);
			CursorAnterior = vec2{ x, y };

			HabilitaMovimentoMouse = true;
		}
		if (acao == GLFW_RELEASE)
		{
			glfwSetInputMode(janela, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			HabilitaMovimentoMouse = false;
		}
	}
}

void MovimentoMouse(GLFWwindow* janela, double x, double y)
{
	if (HabilitaMovimentoMouse == true)
	{
		vec2 CursorAtual{ x, y };
		vec2 Cursor_delta = CursorAtual - CursorAnterior;

		cout << to_string(Cursor_delta) << endl;

		CursorAnterior = CursorAtual;
	}
}

int main()
{
	setlocale(LC_ALL, "Portuguese");
	// Inicia a GLFW e verifica caso ocorra algum erro.
	if (glfwInit() == GLFW_FALSE){

		cout << "Erro ao iniciar o GLFW";
	}

	// Criação da janela principal.
	GLFWwindow* janela = glfwCreateWindow(largura, altura, "The Earth", nullptr, nullptr);
	assert(janela);

	//Cadastra as callbacks do mouse no GLFW
	glfwSetMouseButtonCallback(janela, ClickMouse);
	glfwSetCursorPosCallback(janela, MovimentoMouse);



	//Ativa o contexto criado na janela window
	glfwMakeContextCurrent(janela);

	//Habilita ou não o V-sync.
	glfwSwapInterval(1);

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

	GLuint ProgramaId = CarregaShaders("shaders/triangulo_vert.glsl", "shaders/triangulo_frag.glsl");

	GLuint TextureId = CarregaTextura("texturas/earth_2k.jpg");

	//Definição do triango usando coordenadas normalizadas
	array <Vertex, 6> Quad = {
		Vertex{vec3{-1.0f, -1.0f, 0.0f}, vec3{1.0f, 0.0f, 0.0f}, vec2{0.0f, 0.0f}},
		Vertex{vec3{ 1.0f, -1.0f, 0.0f}, vec3{0.0f, 1.0f, 0.0f}, vec2{1.0f, 0.0f}},
		Vertex{vec3{-1.0f,  1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec2{0.0f, 1.0f}},

		Vertex{vec3{-1.0f,  1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec2{0.0f, 1.0f}},
		Vertex{vec3{ 1.0f, -1.0f, 0.0f}, vec3{0.0f, 1.0f, 0.0f}, vec2{1.0f, 0.0f}},
		Vertex{vec3{ 1.0f,  1.0f, 0.0f}, vec3{0.0f, 1.0f, 0.0f}, vec2{1.0f, 1.0f}}

	};

	//Matriz Modelo
	mat4 MatrizModelo = identity<mat4>();

	//Copiando os vértices do triangulo para a memória da GPU
	GLuint Buffer_Vertices;

	//Gera o identificador do buffer de vertices
	glGenBuffers(1, &Buffer_Vertices);

	//Ativa o buffer de vertices, onde serão copiados os dados do triangulo.
	glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);

	//Copiar os dados para a memoria de video
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	//Define a cor de fundo
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	//Guarda o tempo do frame anterior
	double TempoAnterior = glfwGetTime();

	// Loop de eventos da aplicação.
	while (!glfwWindowShouldClose(janela)) {

		double TempoAtual = glfwGetTime();
		double Tempo_Delta = TempoAtual - TempoAnterior;

		if (Tempo_Delta > 0.0)
		{
			TempoAnterior = TempoAtual;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		//Ativa o programa de shader
		glUseProgram(ProgramaId);

		mat4  ViewProjection = Camera.GetViewProjection();
		mat4 MVP = ViewProjection * MatrizModelo;

		GLint ModelViewProjectionLock = glGetUniformLocation(ProgramaId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLock, 1, GL_FALSE, value_ptr(MVP));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureId);

		GLint TextureSamplerLock = glGetUniformLocation(ProgramaId, "amostraTextura");
		glUniform1i(GL_TEXTURE_2D, TextureId);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//informa o opengl que o Buffer de Vertices vai ser o ativo no momento.
		glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Cor)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

		glDrawArrays(GL_TRIANGLES, 0, Quad.size());

		//Reverter o estado que foi criado
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		//Desabilita o programa ativo.
		glUseProgram(0);

		// Realiza os processamentos em fila do GLFW, e esses eventos podem consistir de teclas apertadas, interação com o mouse etc.
		glfwPollEvents();

		// Envia para ser desenhado na tela o conteúdo do framebuffer.
		glfwSwapBuffers(janela);

		//Processa as entradas do teclado.
		if (glfwGetKey(janela, GLFW_KEY_W) == GLFW_PRESS)
		{
			Camera.emFrente(1.0f * Tempo_Delta);
		}

		if (glfwGetKey(janela, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera.emFrente(-1.0f * Tempo_Delta);
		}

		if (glfwGetKey(janela, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera.aDireita(-1.0f * Tempo_Delta);
		}

		if (glfwGetKey(janela, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera.aDireita(1.0f * Tempo_Delta);
		}

	}

	//Desaloca o Buffer de Vertices
	glDeleteBuffers(1, &Buffer_Vertices);

	// Comando para o encerramento da biblioteca GLFW
	glfwTerminate();


	return 0;

}
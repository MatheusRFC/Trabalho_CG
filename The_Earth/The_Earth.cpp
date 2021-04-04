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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace std;
using namespace glm;

int largura = 1280;
int altura = 720;

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
	vec3 Normal;
	vec3 Cor;
	vec2 UV;
};

struct LuzDirecional
{
	vec3 Direcao;
	GLfloat Intensidade;
};

GLuint CarregaGeometria()
{
	//Definição do triango usando coordenadas normalizadas
	array <Vertex, 6> Quad = {
		Vertex{vec3{-1.0f, -1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec3{1.0f, 0.0f, 0.0f}, vec2{0.0f, 0.0f}},
		Vertex{vec3{ 1.0f, -1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec3{0.0f, 1.0f, 0.0f}, vec2{1.0f, 0.0f}},
		Vertex{vec3{ 1.0f,  1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec3{0.0f, 1.0f, 0.0f}, vec2{1.0f, 1.0f}},
		Vertex{vec3{-1.0f,  1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, vec3{0.0f, 0.0f, 1.0f}, vec2{0.0f, 1.0f}}
	};

	//Define a lista de elementos que formam as listas de triangulos
	array<ivec3, 2> indices = {
		ivec3{0, 1, 3},
		ivec3{3, 1, 2}
	};

	//Copiando os vértices do triangulo para a memória da GPU
	GLuint Buffer_Vertices;

	//Gera o identificador do buffer de vertices
	glGenBuffers(1, &Buffer_Vertices);

	//Falar para o OpenGL gerar o identificador do VBO
	GLuint BufferElementos = 0;
	glGenBuffers(1, &BufferElementos);

	//Ativa o buffer de vertices, onde serão copiados os dados do triangulo.
	glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);

	//Copiar os dados para a memoria de video
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	//Copia os dados do buffer de Elementos pra GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferElementos);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

	//Gera o Vertex Array Object (VAO)
	GLuint VAO; 
	glGenVertexArrays(1, &VAO);

	//Habilita  o VAO
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//informa o opengl que o Buffer de Vertices vai ser o ativo no momento.
	glBindBuffer(GL_ARRAY_BUFFER, Buffer_Vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferElementos);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Cor)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

void GeraEsfera(GLuint resolucao, vector<Vertex>& Vertices, vector<ivec3>& indices)
{
	Vertices.clear();
	indices.clear();

	constexpr float Pi = pi<float>();
	constexpr float DoisPi = two_pi<float>();

	const float InverResolucao = 1.0f / static_cast<float>(resolucao - 1);

	for (GLuint Uindex = 0; Uindex < resolucao; ++Uindex)
	{
		const float U = Uindex * InverResolucao;
		const float Phi = mix(0.0f, DoisPi, U);

		for (GLuint Vindex = 0; Vindex < resolucao; ++Vindex)
		{
			const float V = Vindex * InverResolucao;
			const float Theta = mix(0.0f, Pi, V);

			vec3 VertexPosition =
			{
				sin(Theta) * cos(Phi),
				sin(Theta) * sin(Phi),
				cos(Theta)
			};
			Vertex Vertex
			{
				VertexPosition,
				normalize(VertexPosition),
				vec3(1.0f, 1.0f, 1.0f),
				vec2{1.0f - U, V }
			};

			Vertices.push_back(Vertex);
		}
	}

	for (GLuint U = 0; U < resolucao - 1; ++U)
	{
		for (GLuint V = 0; V < resolucao - 1; ++V)
		{
			GLuint P0 = U + V * resolucao; 
			GLuint P1 = (U + 1) + V * resolucao;
			GLuint P2 = (U + 1) + (V + 1) * resolucao;
			GLuint P3 = U + (V + 1) * resolucao;

			indices.push_back(ivec3{ P0, P1, P3 });
			indices.push_back(ivec3{ P3, P1, P2 });
		}
	}
}

GLuint CarregaEsfera(GLuint &NumVertices, GLuint& NumIndices)
{
	vector<Vertex> Vertices;
	vector<ivec3> Triangulos;
	GeraEsfera(100, Vertices, Triangulos);

	NumVertices = Vertices.size();
	NumIndices = Triangulos.size() * 3;

	GLuint BufferVertices;
	glGenBuffers(1, &BufferVertices);

	//Ativa o buffer de vertices, onde serão copiados os dados do triangulo.
	glBindBuffer(GL_ARRAY_BUFFER, BufferVertices);

	//Copia os dados para a memoria de video
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	GLuint BufferElementos;
	glGenBuffers(1, &BufferElementos);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferElementos);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint), Triangulos.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, BufferVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferElementos);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Cor)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);

	return VAO;
}

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

	void Olhar(float yaw, float pitch)
	{
		yaw *= sensilidade;
		pitch *= sensilidade;

		const vec3 Direita = normalize(cross(Direcao, Cima));

		const mat4 RotacaoYaw = rotate(identity<mat4>(), radians(yaw), Cima);
		const mat4 RotacaoPitch = rotate(identity<mat4>(), radians(pitch), Direita);

		Cima = RotacaoPitch * vec4{ Cima, 0.0f };
		Direcao = RotacaoYaw * RotacaoPitch * vec4{ Direcao, 0.0f };
	}

	mat4 GetView() const
	{
		return lookAt(Localizacao, Localizacao + Direcao, Cima);
	}

	mat4 GetViewProjection() const
	{
		mat4 View = lookAt(Localizacao, Localizacao + Direcao, Cima);
		mat4 Projection = perspective(FieldOfView, AspectRatio, Near, Far);
		return Projection * GetView();
	}

	//Parametros de interatividade
	float velocidade = 20.0f;
	float sensilidade = 0.1f;

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

		//cout << to_string(Cursor_delta) << endl;
		Camera.Olhar(Cursor_delta.x, Cursor_delta.y);

		CursorAnterior = CursorAtual;
	}
}

void Resize(GLFWwindow* janela, int novalargura, int novaaltura)
{
	largura = novalargura;
	altura = novaaltura;

	Camera.AspectRatio = static_cast<float>(largura) / altura;
	glViewport(0, 0, largura, altura);
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
	glfwSetFramebufferSizeCallback(janela, Resize);

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

	Resize(janela, largura, altura);

	GLuint ProgramaId = CarregaShaders("shaders/triangulo_vert.glsl", "shaders/triangulo_frag.glsl");

	GLuint TextureId = CarregaTextura("texturas/earth_2k.jpg");

	GLuint QuadVAO = CarregaGeometria();

	GLuint NumVerticesEsfera = 0;
	GLuint NumIndicesEsfera = 0;

	GLuint VAOEsfera = CarregaEsfera(NumVerticesEsfera, NumIndicesEsfera);

	cout << "NumVert: "<< NumVerticesEsfera << "Num indices: "<< NumIndicesEsfera<< endl;

	//Matriz Modelo
	mat4 I = identity<mat4>();
	mat4 MatrizModelo = rotate(I, radians(90.0f), vec3(1, 0, 0));

	//Define a cor de fundo
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Guarda o tempo do frame anterior
	double TempoAnterior = glfwGetTime();

	//Habilita o backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Cria uma fonte de luz direcional
	LuzDirecional Luz;
	Luz.Direcao = vec3{ 0.0f, 0.0f, -1.0f };
	Luz.Intensidade = 1;

	// Loop de eventos da aplicação.
	while (!glfwWindowShouldClose(janela)) {

		double TempoAtual = glfwGetTime();
		double Tempo_Delta = TempoAtual - TempoAnterior;

		if (Tempo_Delta > 0.0)
		{
			TempoAnterior = TempoAtual;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Ativa o programa de shader
		glUseProgram(ProgramaId);

		mat4 MatrizNormal = inverse(transpose(Camera.GetView() * MatrizModelo));
		mat4 ViewProjection = Camera.GetViewProjection();
		mat4 MVP = ViewProjection * MatrizModelo;

		GLint ModelViewProjectionLock = glGetUniformLocation(ProgramaId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionLock, 1, GL_FALSE, value_ptr(MVP));

		GLint NormalMatrixLock = glGetUniformLocation(ProgramaId, "NormalMatrix");
		glUniformMatrix4fv(NormalMatrixLock, 1, GL_FALSE, value_ptr(MatrizNormal));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureId);

		GLint TextureSamplerLock = glGetUniformLocation(ProgramaId, "amostraTextura");
		glUniform1i(TextureSamplerLock, 0);

		GLint LightDirectionLock = glGetUniformLocation(ProgramaId, "LightDirection");
		glUniform3fv(LightDirectionLock, 1, value_ptr(Camera.GetView() * vec4(Luz.Direcao, 0.0f)));

		GLint LightIntensityLock = glGetUniformLocation(ProgramaId, "LightIntensity");
		glUniform1f(LightIntensityLock, Luz.Intensidade);

		//glBindVertexArray(QuadVAO);
		glBindVertexArray(VAOEsfera);

		//Diz para o OpenGL desenhar o triangulo com os dados armazenados no Buffer de Vertices.
		glPointSize(10.0f);
		glLineWidth(10.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, NumIndicesEsfera, GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_POINTS, 0, NumVerticesEsfera);

		glBindVertexArray(0);

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
	glDeleteVertexArrays(1, &QuadVAO);

	// Comando para o encerramento da biblioteca GLFW
	glfwTerminate();

	return 0;
}
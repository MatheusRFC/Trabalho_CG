#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

void ImprimirMatriz(const mat4& M) {

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout
				<< setw(10)
				<< setprecision(4)
				<< fixed
				<< M[j][i] << " ";
		}
		cout << endl;
	}

}

void MatrizTranslacao() {

	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << "         MATRIZ DE TRANSLACAO         " << endl;
	cout << "--------------------------------------" << endl;

	mat4 I = identity<mat4>();
	vec3 T{ 10, 10, 10 };

	mat4 Translacao = translate(I, T);

	ImprimirMatriz(Translacao);

	vec4 Posicao{ 10, 10, 10, 1 };
	vec4 Direcao{ 10, 10, 10, 0 };

	cout << endl;
	Posicao = Translacao * Posicao;

	cout << to_string(Posicao) << endl;

}

void EscalaMatriz() {

	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << "         ESCALA MATRIZ                " << endl;
	cout << "--------------------------------------" << endl;

	mat4 I = identity<mat4>();
	vec3 QuantidadeE{ 2, 2, 2 };
	mat4 Escala = scale( I, QuantidadeE);

	ImprimirMatriz(Escala);

	vec4 Posicao{ 100, 100, 0, 1 };
	vec4 Direcao{ 100, 100, 0, 0 };

	Posicao = Escala * Posicao;
	cout << endl;
	cout << to_string(Posicao) << endl;

}

void MatrizRotacao() {
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << "         ROTACAO MATRIZ               " << endl;
	cout << "--------------------------------------" << endl;

	mat4 I = identity<mat4>();
	constexpr float angulo = radians(90.0f);
	vec3 eixo{ 0, 0, 1 };

	mat4 rotacao = rotate(I, angulo, eixo);

	ImprimirMatriz(rotacao);

	vec4 posicao{ 100, 0, 0, 1 };
	vec4 direcao{ 10, 0, 0, 0 };

	posicao = rotacao * posicao;

	cout << to_string(posicao) << endl;

}

void MatrizComposicao() {
	cout << endl;
	cout << "--------------------------------------" << endl;
	cout << "         COMPOSICAO MATRIZ            " << endl;
	cout << "--------------------------------------" << endl;

	mat4 I = identity<mat4>();

	vec3 T{ 0, 10, 0 };
	mat4 translacao = translate(I, T);

	constexpr float angulo = radians(45.0f);
	vec3 eixo{ 0, 0, 1 };

	mat4 rotacao = rotate(I, angulo, eixo);

	vec3 QuantidadeE{ 2, 2, 0 };
	mat4 Escala = scale(I, QuantidadeE);

	cout << "Translacao" << endl;
	ImprimirMatriz(translacao);
	cout << endl;

	cout << "Rotacao" << endl;
	ImprimirMatriz(rotacao);
	cout << endl;

	cout << "Escala" << endl;
	ImprimirMatriz(Escala);
	cout << endl;

}

void MVP() {
	cout << "--------------------------------------" << endl;
	cout << "         MODEL VIEW PROJECTION        " << endl;
	cout << "--------------------------------------" << endl;

	mat4 model = identity<mat4>();


}

int main() {

	MatrizComposicao();

	return 0;
}
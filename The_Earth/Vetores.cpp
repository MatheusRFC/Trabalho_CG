#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;

void Construtores() {

	cout << "\n";

	glm::vec2 Ponto0{ 10.0f, 10.0f};
	glm::vec3 Ponto1{ 10.0f, 10.0f, 0.0f};
	glm::vec4 Ponto2{ 10.0f, 10.0f, 0.0f, 0.0f};

	cout << "Ponto1: " << glm::to_string(Ponto1) << "\n";
	cout << "Ponto0: " << glm::to_string(Ponto0) << "\n";
	cout << "Ponto2: " << glm::to_string(Ponto2) << "\n";
}

void componentes() {
	cout << "\n";

	glm:: vec3 P{ 1, 2, 3 };

	cout << "X: " << P.x << "Y: " << P.y << "Z: P.z";

}

int main(){

	Construtores();
	componentes();
	return 0;
}
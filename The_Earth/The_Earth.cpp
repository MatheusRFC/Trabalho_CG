#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <stb_image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

int main()
{
	glm::vec3 V{ 1, 2, 3 };

	glfwInit();
	glewInit();

	cout << glm::to_string(V) << endl;
	return 0;

}
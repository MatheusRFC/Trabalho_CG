# CMakeList.txt: projeto de CMake para The_Earth, incluir origem e definir
# lógica específica a um projeto aqui.
#
cmake_minimum_required (VERSION 3.19)

project ("The_Earth")

# Adicione a origem ao executável deste projeto.
add_executable (The_Earth "The_Earth.cpp")
add_executable (Vetores "Vetores.cpp")
add_executable (Matrizes "Matrizes.cpp")

# TODO: Adicione testes e instale destinos, se necessário.
target_include_directories(The_Earth PRIVATE deps/glm deps/stb deps/glfw/include deps/glew/include)

target_include_directories(Vetores PRIVATE  deps/glm)

target_include_directories(Matrizes PRIVATE deps/glm)

target_link_directories(The_Earth PRIVATE deps/glfw/lib-vc2019 deps/glew/lib/Release/x64)

target_link_libraries(The_Earth PRIVATE glfw3.lib glew32.lib opengl32.lib)

add_custom_command(TARGET The_Earth POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/glew/bin/Release/x64/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll" COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders" COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/texturas" "${CMAKE_BINARY_DIR}/texturas")
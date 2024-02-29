#include "Renderer_OpenGL.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VertexArray.hpp"
#include <iostream>

namespace SimpleEngine {

	bool Renderer_OpenGL::init(GLFWwindow* pWindow) {

		glfwMakeContextCurrent(pWindow);//Задаём текущим контекст OpenGL

		//Инициализируем ГЛАД
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			std::cerr << "Failed to initialize GLAD" << "\n";
			return false;
		}

		std::cout << "OpenGL context initialized:\n";
		std::cout << "  Vendor: " << get_vendor_str() << "\n";
		std::cout << "  Renderer: " << get_renderer_str() << "\n";
		std::cout << "  Version: " << get_version_str() << "\n";

		return true;
	}

	void Renderer_OpenGL::draw(const VertexArray& vertex_array) {

		vertex_array.bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertex_array.get_indexes_count()), GL_UNSIGNED_INT, nullptr);
	}

	void Renderer_OpenGL::set_clear_color(const float r, const float g, const float b, const float a) {

		glClearColor(r, g, b, a);
	}

	void Renderer_OpenGL::clear() {

		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer_OpenGL::set_viewport(const unsigned int width, const unsigned int height, const unsigned int left_offset, const unsigned int bottom_offset) {

		glViewport(left_offset, bottom_offset, width, height);
	}

	const char* Renderer_OpenGL::get_vendor_str() {

		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}

	const char* Renderer_OpenGL::get_renderer_str() {

		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* Renderer_OpenGL::get_version_str() {

		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}
}
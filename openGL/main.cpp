#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define WIDTH 800
#define HEIGHT 600

static GLFWwindow* window;

int main() {
	if (!glfwInit()) {
		std::cout << "Failed to intiialize GLFW!" << std::endl;
		return -1;
	}

	/* Setting's */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello, Triangle!", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window!" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW!" << std::endl;
		return -1;
	}

	/* Data */
	float positions[12] = 
	{
		 -0.5f,  0.5f, 0.0f, // left upper corner
		 -0.5f, -0.5f, 0.0f, // left lower corner
		  0.5f,  0.5f, 0.0f, // right upper corner
		  0.5f, -0.5f, 0.0f  // right lower corner
	};

	unsigned int indices[6] = {
		0, 1, 2,
		1, 2, 3
	};


	/* Initializing object's */
	VertexArray va;

	VertexBuffer vb(positions, 4 * 3 * sizeof(float));
	VertexBufferLayout layout;

	layout.Push<float>(3);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 2 * 3 * sizeof(unsigned int));

	Shader shader("default.shader");
	shader.Bind();

	/* UnBinding all element's */
	shader.UnBind();
	va.UnBind();
	vb.UnBind();
	ib.UnBind();

	Renderer renderer;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		/* Render */
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.0f, 1.0);

		renderer.Draw(va, ib, shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}
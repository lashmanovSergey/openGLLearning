#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


#define WIDTH 800
#define HEIGHT 600

static GLFWwindow* window;

enum class ShaderType
{
	NONE = 0, VERTEX = 0, FRAGMENT = 1
};
struct ShaderProgramSource
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::stringstream ss[2];
	std::string line;
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" 
			<< std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main() {
	// initializing GLFW.
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

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	VertexBuffer vb(positions, 4 * 3 * sizeof(float));

	GlCall(glEnableVertexAttribArray(0));
	GlCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));

	IndexBuffer ib(indices, 2 * 3 * sizeof(unsigned int));

	/* shader program */
	ShaderProgramSource source = ParseShader("default.shader");
	unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);
	glUniform4f(location, 0.2f, 0.3f, 0.0f, 1.0f);

	glBindVertexArray(0);
	glUseProgram(0);
	vb.UnBind();
	ib.UnBind();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(vao);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}
#include "Shader.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader();
	m_RendererID = CreateShader(source.vertexSource, source.fragmentSource);
}
Shader::~Shader()
{
	GlCall(glDeleteShader(m_RendererID));
}

void Shader::Bind() const
{
	GlCall(glUseProgram(m_RendererID));
}
void Shader::UnBind() const
{
	GlCall(glUseProgram(0));
}

ShaderProgramSource Shader::ParseShader()
{
	std::ifstream stream(m_FilePath);

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

void Shader::SetUniform1i(const std::string& name, int v0)
{
	GlCall(glUniform1i(GetUniformLocation(name), v0));
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	GlCall(glUniform1f(GetUniformLocation(name), v0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GlCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GlCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
	
	m_UniformLocationCache[name] = location;

	return location;
}


unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!"
			<< std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

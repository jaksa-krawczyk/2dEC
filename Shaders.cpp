#include "Shaders.hpp"

#include <iostream>

Shaders::Shaders()
{
	vertexShader = "#version 330 core\n"
		"layout(location = 0) in vec4 positions;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = positions;\n"
		"}\n";

	fragmentShader = "#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}\n";
}

std::optional<GLuint> Shaders::compileShader(const std::string_view shader, const GLenum shaderType) noexcept
{
	GLuint shaderId = glCreateShader(shaderType);
	const char* shaderPtr = shader.data();
	glShaderSource(shaderId, 1, &shaderPtr, NULL);
	glCompileShader(shaderId);

	GLint compileResult;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);
	if (compileResult != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar log[1024];

		glGetShaderInfoLog(shaderId, 1024, &logLength, log);
		std::cout << "SHADER COMPILATION ERROR(GLenum = " << shaderType << ")" << ": " << log << "\n";
		return std::nullopt;
	}

	return shaderId;
}

bool Shaders::queryGetProgramStatus(const GLuint programId, const GLenum infoType) noexcept
{
	GLint status;
	glGetProgramiv(programId, infoType, &status);
	if (status != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar log[1024];

		glGetProgramInfoLog(programId, 1024, &logLength, log);
		std::cout << "PROGRAM STATUS ERROR(GLenum = " << infoType << ")" << ": " << log << "\n";
		return false;
	}
	return true;
}

std::optional<GLuint> Shaders::getProgramId() noexcept
{
	GLuint programId = glCreateProgram();

	auto fragmentShaderId = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
	if (!fragmentShaderId.has_value())
	{
		return std::nullopt;
	}

	auto vertexShaderId = compileShader(vertexShader, GL_VERTEX_SHADER);
	if (!vertexShaderId.has_value())
	{
		glDeleteShader(fragmentShaderId.value());
		return std::nullopt;
	}

	glAttachShader(programId, fragmentShaderId.value());
	glAttachShader(programId, vertexShaderId.value());

	glLinkProgram(programId);
	if (!queryGetProgramStatus(programId, GL_LINK_STATUS))
	{
		glDeleteShader(fragmentShaderId.value());
		glDeleteShader(vertexShaderId.value());
		return std::nullopt;
	}

	glValidateProgram(programId);
	if (!queryGetProgramStatus(programId, GL_VALIDATE_STATUS))
	{
		glDeleteShader(fragmentShaderId.value());
		glDeleteShader(vertexShaderId.value());
		return std::nullopt;
	}

	glDeleteShader(fragmentShaderId.value());
	glDeleteShader(vertexShaderId.value());
	return programId;
}
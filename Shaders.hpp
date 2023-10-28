#ifndef SHADERS_HPP
#define SHADERS_HPP

#include<GL/glew.h>

#include <optional>
#include <string>
#include <string_view>

class Shaders
{
private:
	std::string fragmentShader;
	std::string vertexShader;
	std::optional<GLuint> compileShader(const std::string_view shader, const GLenum shaderType) noexcept;
	bool queryGetProgramStatus(const GLuint programId, const GLenum infoType) noexcept;
public:
	Shaders();
	std::optional<GLuint> getProgramId() noexcept;
};

#endif
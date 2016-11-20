#ifndef _WDGS_GRAPHICS_SHADER_H
#define _WDGS_GRAPHICS_SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "debug.h"
#include "memmng.h"

namespace WDGS
{
	namespace Graphics
	{
		class Shader
		{
			DECLARE_MEMMNG(Shader)

		public:
			static Ptr CreateFromString(GLuint shaderType, const GLchar* str)
			{
				Ptr ptr = Create();

				ptr->type = shaderType;
				ptr->id = glCreateShader(shaderType);
				glShaderSource(ptr->id, 1, &str, NULL);
				glCompileShader(ptr->id);

				ptr->OutputLog();

				return ptr;
			}

			static Ptr CreateFromFile(GLuint shaderType, const GLchar* path)
			{
				std::string code;
				std::ifstream shaderFile;

				shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

				try
				{
					shaderFile.open(path);
					std::stringstream shaderStream;
					shaderStream << shaderFile.rdbuf();

					shaderFile.close();
					code = shaderStream.str();
				}
				catch (std::ifstream::failure e)
				{
					cdbg << "Unable to read shader file " << path << std::endl;
				}

				return CreateFromString(shaderType, code.c_str());
			}

		protected:
			GLuint id;
			GLuint type;
			Shader() { id = 0; type = 0; }

		public:

			operator GLuint() const 
			{
				return this->id;
			}

			GLuint GetType() { return type; }

			bool operator==(const Shader& other)
			{
				return this->id == other.id;
			}

			~Shader()
			{
				if (id)
				{
					glDeleteShader((GLuint)id);
					id = 0;
				}
			}

		private:

			void OutputLog()
			{
				GLint success;
				GLchar infoLog[1024];

				glGetShaderiv(*this, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(*this, 1024, NULL, infoLog);
					cdbg << "Shader id(" << *this << ") compilation error:\n" << infoLog << "\n----------------------------------------------------" << std::endl;
				}
			}
		};
	}
}

#endif
#ifndef _WDGS_GRAPHICS_PROGRAM_H
#define _WDGS_GRAPHICS_PROGRAM_H

#include <vector>
#include <map>

#include "graphics/shader.h"
#include "memmng.h"

namespace WDGS
{
	namespace Graphics
	{
		class Program
		{
			DECLARE_MEMMNG(Program)

		public:
			static Ptr Create(std::vector<Shader::Ptr>& shaders)
			{
				Ptr ptr = Create();
				for (size_t i = 0; i < shaders.size(); ++i)
					ptr->AddShader(shaders[i]);

				return ptr;
			}

		protected:
			GLuint id;
			std::vector<Shader::Ptr> shaders;

			Program() { id = glCreateProgram(); }

		public:
			void AddShader(Shader::Ptr& sh)
			{
				if (id)
				{
					shaders.push_back(sh);
					glAttachShader(id, *sh);
				}
			}

			void RemoveShader(Shader::Ptr& sh)
			{
				if (id)				
				{
					auto it = std::find_if(shaders.begin(), shaders.end(), std::bind2nd(ptr_equals<Shader::Ptr>(), sh));
					if (it != shaders.end())
					{
						shaders.erase(it);
						glDetachShader(*this, *sh);
					}
				}
			}

			operator GLuint()
			{
				return this->id;
			}

			GLint Link(bool releaseShaders = true)
			{
				if (id)
				{
					glLinkProgram(*this);

					GLint success;
					glGetProgramiv(*this, GL_LINK_STATUS, &success);

					if (releaseShaders)
						shaders.clear();

					OutputLog();
					return success;
				}

				return GL_FALSE;
			}

			void Use()
			{
				if (id)
				{
					glUseProgram(*this);
				}
			}

			~Program()
			{
				if (id)
				{
					glDeleteProgram(id);
					id = 0;
				}
			}


			void OutputLog()
			{
				GLint success;
				GLchar infoLog[1024];

				glGetProgramiv(*this, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(*this, 1024, NULL, infoLog);
					cdbg << "Program id(" << *this << ") linking error:\n" << infoLog << "\n----------------------------------------------------" << std::endl;
				}
				
			}

		};
	}
}

#endif
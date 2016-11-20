#ifndef _WDGS_GRAPHICS_MESH_H
#define _WDGS_GRAPHICS_MESH_H

#include <vector>

#include "memmng.h"
#include "graphics/program.h"
#include "graphics/texture.h"

#include <dirent.h>

namespace WDGS
{
	namespace Graphics
	{
		class Mesh 
		{
			DECLARE_MEMMNG(Mesh)

		protected:
			struct TexEntry
			{
				Texture::Ptr tex;
				std::string sampler;
				GLint location;
			};

			std::vector<TexEntry> textures;
			GLuint vao;
			GLsizei count;

			Program::Ptr renderProg;

			Mesh()
			{
				glGenVertexArrays(1, &this->vao);

				this->count = 0;
			}

		public:
			GLuint GetVAO()
			{
				return this->vao;
			}

			GLsizei GetCount() { return count; }

			void BindTextures()
			{
				renderProg->Use();

				for (GLuint i = 0; i < this->textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					glUniform1i(textures[i].location, i);
					this->textures[i].tex->Bind();
				}
			}

			void SetProgram(Program::Ptr& prog)
			{
				renderProg = prog;
				for (size_t i = 0; i < textures.size(); ++i)
				{
					textures[i].location = glGetUniformLocation(*prog, textures[i].sampler.c_str());					
				}
			}

			Program::Ptr& GetProgram()
			{
				return renderProg;
			}

			void AddTexture(Texture::Ptr& tex, const char* sampler)
			{
				TexEntry te;
				te.tex = tex;
				te.sampler = sampler;

				if (renderProg)
					te.location = glGetUniformLocation(*renderProg, sampler);
				else
					te.location = 0;

				textures.push_back(te);
			}

			void RemoveTexure(Texture::Ptr& tex, const char* sampler)
			{
				auto newEnd = std::remove_if(textures.begin(), textures.end(), [&tex, &sampler](TexEntry& e) {return e.tex == tex && !strcmp(e.sampler.c_str(), sampler); });
				textures.erase(newEnd, textures.end());

			}

			void AddTexturesFromFolder(const GLchar* path)
			{
				DIR *dir;
				struct dirent *ent;
				char fullname[PATH_MAX];

				if ((dir = opendir(path)) != NULL) 
				{
					while ((ent = readdir(dir)) != NULL) 
					{
						if (ent->d_type == DT_REG)
						{
							strcpy(fullname, path);
							if (fullname[strlen(fullname) - 1] != '/')
								strcat(fullname, "/");

							strcat(fullname, ent->d_name);

							Texture::Ptr tex = Texture::Create(fullname);
							char* p = strrchr(ent->d_name, '.');
							*p = 0;

							AddTexture(tex, ent->d_name);
						}
					}
					closedir(dir);
				}
			}

			void ClearTextures()
			{
				textures.clear();
			}

			virtual ~Mesh()
			{
				glDeleteVertexArrays(1, &this->vao);
			}
		};


		class Sphere : public Mesh
		{
			DECLARE_MEMMNG(Sphere)

		public:
			static Ptr Create(int ccw, int level)
			{
				Ptr ptr = Create();

				ptr->ccw = ccw;
				ptr->maxlevel = level;

				glBindVertexArray(ptr->vao);

				static vertex xplus{ {1.0f, 0.0f, 0.0f} };		/*  X */
				static vertex xminus{ {-1.0f, 0.0f, 0.0f} };	/* -X */
				static vertex yplus{ {0.0f, 1.0f, 0.0f} };		/*  Y */
				static vertex yminus{ {0.0f, -1.0f, 0.0f} };	/* -Y */
				static vertex zplus{ {0.0f, 0.0f, 1.0f} };		/*  Z */
				static vertex zminus{ {0.0f, 0.0f, -1.0f} };	/* -Z */

				/* Vertices of a unit octahedron */
				static triangle octahedron[] = {
					{ { xplus, zplus, yplus } },
					{ { yplus, zplus, xminus } },
					{ { xminus , zplus, yminus } },
					{ { yminus , zplus, xplus } },
					{ { xplus, yplus, zminus } },
					{ { yplus, xminus , zminus } },
					{ { xminus , yminus , zminus } },
					{ { yminus , xplus, zminus } }
				};


				glGenBuffers(1, &ptr->vbo);
				glBindBuffer(GL_ARRAY_BUFFER, ptr->vbo);

				int step = 1;
				for (int level = 1; level < ptr->maxlevel; ++level)
					step *= 4;

				ptr->count = sizeof(octahedron) * step;
				glBufferData(GL_ARRAY_BUFFER, ptr->count, 0, GL_STATIC_DRAW);
				triangle* buf = (triangle*)glMapBufferRange(GL_ARRAY_BUFFER, 0, ptr->count, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
				int size = sizeof(octahedron) / sizeof(octahedron[0]) * step;
				for (int i = step - 1, j = 0; i < size; i += step, ++j)
				{
					if (ccw)
					{
						buf[i].v[2].position = octahedron[j].v[0].position;
						buf[i].v[1].position = octahedron[j].v[1].position;
						buf[i].v[0].position = octahedron[j].v[2].position;
					}
					else
					{
						buf[i].v[0].position = octahedron[j].v[0].position;
						buf[i].v[1].position = octahedron[j].v[1].position;
						buf[i].v[2].position = octahedron[j].v[2].position;
					}
				}

				for (int level = 1; level < ptr->maxlevel; level++)
				{
					for (int i = step - 1, j = 0; i < size; i += step, ++j)
					{
						triangle *oldt = &buf[i], *newt = &buf[j*step + (step / 4) - 1];

						glm::vec3 a, b, c;

						a = glm::normalize(midpoint(oldt->v[0].position, oldt->v[2].position));
						b = glm::normalize(midpoint(oldt->v[0].position, oldt->v[1].position));
						c = glm::normalize(midpoint(oldt->v[1].position, oldt->v[2].position));

						newt->v[0].position = oldt->v[0].position;
						newt->v[1].position = b;
						newt->v[2].position = a;

						newt += step / 4;

						newt->v[0].position = b;
						newt->v[1].position = oldt->v[1].position;
						newt->v[2].position = c;

						newt += step / 4;

						newt->v[0].position = a;
						newt->v[1].position = b;
						newt->v[2].position = c;

						newt += step / 4;

						newt->v[0].position = a;
						newt->v[1].position = c;
						newt->v[2].position = oldt->v[2].position;

					}

					step /= 4;
				}

				glUnmapBuffer(GL_ARRAY_BUFFER);
			

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0);	

				glBindVertexArray(0);

				return ptr;
			}

		protected:
			int ccw, maxlevel;
			GLuint vbo;

			struct vertex
			{
				glm::vec3 position;
			};

			struct triangle {
				vertex     v[3];
			};

			static glm::vec3 midpoint(glm::vec3& v1, glm::vec3& v2)
			{
				glm::vec3 res;
				res[0] = (v1[0] + v2[0]) * 0.5f;
				res[1] = (v1[1] + v2[1]) * 0.5f;
				res[2] = (v1[2] + v2[2]) * 0.5f;

				return res;
			}


			Sphere() : Mesh()
			{
				ccw = 0; maxlevel = 0;
				vbo = 0;
			}

			public:
			~Sphere()
			{
				glDeleteBuffers(1, &vbo);
			}
		};
	}
}

#endif
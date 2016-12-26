#ifndef _WDGS_GRAPHICS_MESH_H
#define _WDGS_GRAPHICS_MESH_H

#include "memmng.h"
#include "graphics/program.h"
#include "graphics/texture.h"

#define SPHERE_MAX_DETALIZATION 6


namespace WDGS
{
	namespace Graphics
	{
		class MeshBase
		{
			DECLARE_MEMMNG_NOCREATE(MeshBase)

		protected:
			struct TexEntry
			{
				Texture::Ptr tex;
				std::string sampler;
				GLint location;
			};

			std::vector<TexEntry> textures;

			Program::Ptr renderProg;

			MeshBase()
			{
				renderProg = 0;
			}

		public:
			virtual GLuint GetVAO() = 0;
			virtual GLsizei GetCount() = 0;
			virtual GLenum GetMode() = 0;

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

			void UnbindTextures()
			{
				renderProg->Use();

				for (GLuint i = 0; i < this->textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					glUniform1i(textures[i].location, 0);
					glBindTexture(this->textures[i].tex->GetTarget(), 0);
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

			virtual void Render()
			{
				BindTextures();

				glBindVertexArray(GetVAO());
				glDrawArrays(GetMode(), 0, GetCount());

				//UnbindTextures();
			}

			void ClearTextures()
			{
				textures.clear();
			}

			virtual ~MeshBase() {}
		};

		class Quad : public MeshBase
		{
			DECLARE_MEMMNG(Quad)

		protected:
			static GLuint vao;
			static GLuint vbo;
			static GLuint instances;

			Quad() : MeshBase()
			{
				if (instances == 0)
				{
					glGenVertexArrays(1, &Quad::vao);
					glGenBuffers(1, &Quad::vbo);

					GLfloat vertices[] = { 
						-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
						1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
						-1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
						1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
					};

					glBindVertexArray(Quad::vao);
					glBindBuffer(GL_ARRAY_BUFFER, Quad::vbo);
					glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}

				++instances;
			}

		public:

			virtual GLuint GetVAO() { return Quad::vao; }
			virtual GLsizei GetCount() { return 4; }
			virtual GLenum GetMode() { return GL_TRIANGLE_STRIP; }

			virtual ~Quad()
			{
				if (instances == 1)
				{
					glDeleteVertexArrays(1, &Quad::vao);
					glDeleteBuffers(1, &Quad::vbo);
				}

				if (instances > 0) --instances;
			}
		};

		class Cube : public MeshBase
		{
			DECLARE_MEMMNG(Cube)

		protected:
			static GLuint vao;
			static GLuint vbo;
			static GLuint instances;

			Cube() : MeshBase()
			{
				if (instances == 0)
				{
					glGenVertexArrays(1, &Cube::vao);
					glGenBuffers(1, &Cube::vbo);

					GLfloat vertices[] = {
						// Positions          
						-1.0f, 1.0f, -1.0f,
						-1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f, 1.0f, -1.0f,
						-1.0f, 1.0f, -1.0f,

						-1.0f, -1.0f, 1.0f,
						-1.0f, -1.0f, -1.0f,
						-1.0f, 1.0f, -1.0f,
						-1.0f, 1.0f, -1.0f,
						-1.0f, 1.0f, 1.0f,
						-1.0f, -1.0f, 1.0f,

						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, 1.0f,
						1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,

						-1.0f, -1.0f, 1.0f,
						-1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f,
						1.0f, -1.0f, 1.0f,
						-1.0f, -1.0f, 1.0f,

						-1.0f, 1.0f, -1.0f,
						1.0f, 1.0f, -1.0f,
						1.0f, 1.0f, 1.0f,
						1.0f, 1.0f, 1.0f,
						-1.0f, 1.0f, 1.0f,
						-1.0f, 1.0f, -1.0f,

						-1.0f, -1.0f, -1.0f,
						-1.0f, -1.0f, 1.0f,
						1.0f, -1.0f, -1.0f,
						1.0f, -1.0f, -1.0f,
						-1.0f, -1.0f, 1.0f,
						1.0f, -1.0f, 1.0f
					};

					glBindVertexArray(Cube::vao);
					glBindBuffer(GL_ARRAY_BUFFER, Cube::vbo);
					glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}

				++instances;
			}

		public:

			virtual GLuint GetVAO() { return Cube::vao; }
			virtual GLsizei GetCount() { return 36; }
			virtual GLenum GetMode() { return GL_TRIANGLES; }

			virtual ~Cube()
			{
				if (instances == 1)
				{
					glDeleteVertexArrays(1, &Cube::vao);
					glDeleteBuffers(1, &Cube::vbo);
				}

				if (instances > 0) --instances;
			}
		};

		class Sphere : public MeshBase
		{
			DECLARE_MEMMNG(Sphere)

		protected:
			/*static*/ GLuint vao;
			static GLuint vbo;
			static GLuint instances;
			static GLsizei count;

			int maxlevel;

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

			Sphere() : MeshBase()
			{

				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);


				maxlevel = SPHERE_MAX_DETALIZATION;

				if (instances == 0)
				{
					vertex xplus;
					xplus.position = glm::vec3(1.0f, 0.0f, 0.0f);		/*  X */
					vertex xminus;
					xminus.position = glm::vec3(-1.0f, 0.0f, 0.0f);		/*  X */
					vertex yplus;
					yplus.position = glm::vec3(0.0f, 1.0f, 0.0f);		/*  X */
					vertex yminus;
					yminus.position = glm::vec3(0.0f, -1.0f, 0.0f);		/*  X */
					vertex zplus;
					zplus.position = glm::vec3(0.0f, 0.0f, 1.0f);		/*  X */
					vertex zminus;
					zminus.position = glm::vec3(0.0f, 0.0f, -1.0f);		/*  X */

					triangle octahedron[] = {
						{ { xplus, zplus, yplus } },
						{ { yplus, zplus, xminus } },
						{ { xminus , zplus, yminus } },
						{ { yminus , zplus, xplus } },
						{ { xplus, yplus, zminus } },
						{ { yplus, xminus , zminus } },
						{ { xminus , yminus , zminus } },
						{ { yminus , xplus, zminus } }
					};

					glGenBuffers(1, &Sphere::vbo);

					int ccw = 1;

					int step = 1;
					for (int level = 0; level < maxlevel; ++level)
						step *= 4;

					count = sizeof(octahedron) * step;

					//glBindVertexArray(Sphere::vao);
					glBindBuffer(GL_ARRAY_BUFFER, Sphere::vbo);

					glBufferData(GL_ARRAY_BUFFER, count, 0, GL_STATIC_DRAW);
					triangle* buf = (triangle*)glMapBufferRange(GL_ARRAY_BUFFER, 0, count, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
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

					for (int level = 0; level < maxlevel; ++level)
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
							newt->v[1].position = c;
							newt->v[2].position = oldt->v[2].position;

							newt += step / 4;

							newt->v[0].position = a;
							newt->v[1].position = b;
							newt->v[2].position = c;

						}

						step /= 4;
					}

					for (int i = 0; i < size; ++i)
					{
						if (((vertex*)buf)[i].position == zplus.position || ((vertex*)buf)[i].position == yplus.position
							|| ((vertex*)buf)[i].position == xplus.position ||
							((vertex*)buf)[i].position == zminus.position
							|| ((vertex*)buf)[i].position == yminus.position
							|| ((vertex*)buf)[i].position == xminus.position)
							cdbg << i << "\n";
					}

					glUnmapBuffer(GL_ARRAY_BUFFER);

					
				}

				//SetDetalizationLevel(1);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0);

				glBindVertexArray(0);

				++instances;
			}

		public:

			virtual GLuint GetVAO() { return vao; }
			virtual GLsizei GetCount() 
			{ 
				/*GLint cnt = Sphere::count;
				for (int l = maxlevel; l <= SPHERE_MAX_DETALIZATION; ++l)
					cnt /=4;*/

				return Sphere::count;
			}

			virtual GLenum GetMode() { return GL_TRIANGLES; }

			void SetDetalizationLevel(int level)
			{
				maxlevel = level;

				int step = 1;
				for (int l = maxlevel; l < SPHERE_MAX_DETALIZATION; ++l)
					step *= 4;

				//int first = 0;
				//int ss = 1;
				//for (int l = maxlevel; l < SPHERE_MAX_DETALIZATION; ++l)
				//{
				//	first = step - 1;
				//	step /= 4;
				//	ss *= 4;
				//}

				//int first;
				//int size = GetCount() / sizeof(triangle);

				//int step = 1;
				//for (int l = 0; l < maxlevel; ++l)
				//	step *= 4;

				//for (int l = 0; l < maxlevel; ++l)
				//{
				//	//for (int i = step - 1, j = 0; i < size; i += step, ++j)
				//	//{
				//		first = (step / 4) - 1;
				//	//}
				//}
				/*int step = sizeof(vertex);
				for (int i = maxlevel; i < SPHERE_MAX_DETALIZATION; ++i)
					step *= 4;

				int a = (step / sizeof(vertex) - 1) * sizeof(vertex);*/
			
			// GL_MAX_VERTEX_ATTRIB_STRIDE
				//GLint d;
				//glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE,
				//	&d);

				//glEnableVertexAttribArray(0);
				//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4000, (GLvoid*)(0));

			}

			virtual ~Sphere()
			{
				glDeleteVertexArrays(1, &vao);

				if (instances == 1)
				{
					//glDeleteVertexArrays(1, &Sphere::vao);
					glDeleteBuffers(1, &Sphere::vbo);
				}

				if (instances > 0) --instances;
			}
		};
	}
}

#endif
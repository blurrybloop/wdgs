#ifndef _WDGS_GRAPHICS_TEXTURE_H
#define _WDGS_GRAPHICS_TEXTURE_H

#include "memmng.h"
#include "graphics/program.h"

namespace WDGS
{
	namespace Graphics
	{
		class Texture
		{
			DECLARE_MEMMNG(Texture)

		public:

			static Ptr Create(GLenum target)
			{
				Ptr ptr = Create();

				ptr->target = target;

				glGenTextures(1, &ptr->id);
				glBindTexture(ptr->target, ptr->id);

				return ptr;
			}

			static Ptr Create(const GLchar* path)
			{
				Ptr ptr = Create();

				gli::texture tex = gli::load(path);

				if (tex.empty())
					return ptr;

				gli::gl GL(gli::gl::PROFILE_GL32);
				gli::gl::format const fmt = GL.translate(tex.format(), tex.swizzles());
				ptr->target = GL.translate(tex.target());

				glGenTextures(1, &ptr->id);
				glBindTexture(ptr->target, *ptr);

				glTexParameteri(ptr->target, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(ptr->target, GL_TEXTURE_MAX_LEVEL, tex.levels() - 1);
				glTexParameteri(ptr->target, GL_TEXTURE_SWIZZLE_R, fmt.Swizzles[0]);
				glTexParameteri(ptr->target, GL_TEXTURE_SWIZZLE_G, fmt.Swizzles[1]);
				glTexParameteri(ptr->target, GL_TEXTURE_SWIZZLE_B, fmt.Swizzles[2]);
				glTexParameteri(ptr->target, GL_TEXTURE_SWIZZLE_A, fmt.Swizzles[3]);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				glm::tvec3<GLsizei> const extent(tex.extent());

				for (std::size_t layer = 0; layer < tex.layers(); ++layer)
					for (std::size_t face = 0; face < tex.faces(); ++face)
						for (std::size_t level = 0; level < tex.levels(); ++level)
						{

							GLsizei const LayerGL = static_cast<GLsizei>(layer);
							glm::tvec3<GLsizei> extent(tex.extent(level));
							GLuint t = gli::is_target_cube(tex.target())
								? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
								: ptr->target;

							switch (tex.target())
							{
							case gli::TARGET_1D:
								if (gli::is_compressed(tex.format()))
									glCompressedTexImage1D(
										t, static_cast<GLint>(level), fmt.Internal, extent.x, 0,
										static_cast<GLsizei>(tex.size(level)),
										tex.data(layer, face, level));
								else
									glTexImage1D(
										t, static_cast<GLint>(level), fmt.Internal, extent.x,
										0, fmt.External, fmt.Type,
										tex.data(layer, face, level));
								break;
							case gli::TARGET_1D_ARRAY:
							case gli::TARGET_2D:
							case gli::TARGET_CUBE:
								if (gli::is_compressed(tex.format()))
								{
									glCompressedTexImage2D(
										t, level,
										fmt.Internal,
										extent.x, extent.y,
										0,
										static_cast<GLsizei>(tex.size(level)),
										tex.data(layer, face, level));
								}
								else
									glTexImage2D(
										t, level,
										fmt.Internal, extent.x, extent.y, 0, fmt.External, fmt.Type, tex.data(layer, face, level));
								break;

							case gli::TARGET_2D_ARRAY:
							case gli::TARGET_3D:
							case gli::TARGET_CUBE_ARRAY:
								if (gli::is_compressed(tex.format()))
									glCompressedTexImage3D(
										t, static_cast<GLint>(level), fmt.Internal,
										extent.x, extent.y, tex.target() == gli::TARGET_3D ? extent.z : LayerGL, 0,
										static_cast<GLsizei>(tex.size(level)),
										tex.data(layer, face, level));
								else
									glTexImage3D(
										t, static_cast<GLint>(level), fmt.Internal,
										extent.x, extent.y, tex.target() == gli::TARGET_3D ? extent.z : LayerGL, 0,
										fmt.External, fmt.Type,
										tex.data(layer, face, level));
								break;

							default:
								break;
							}
						}

				return ptr;
			}

		protected:
			GLuint id;
			GLenum target;

			Texture() { id = 0; target = 0; }

		public:

			void Bind()
			{
				if (id)
				{
					glBindTexture(target, *this);
				}
			}

			operator GLuint()
			{
				return this->id;
			}

			~Texture()
			{
				if (id)
				{
					glDeleteTextures(1, &id);
					id = 0;
				}
			}

		};
	}
}

#endif

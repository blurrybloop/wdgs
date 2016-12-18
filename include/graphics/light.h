#ifndef _WDGS_GRAPHICS_LIGHT_H
#define _WDGS_GRAPHICS_LIGHT_H

namespace WDGS
{
	namespace Graphics
	{
		class Light
		{
		public:

			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			glm::vec3 position;
		};
	}
}

#endif

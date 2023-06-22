#ifndef MODEL3D_H
#define MODEL3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace model {
	class Model3D {
		public:
			std::string path;
			float x, y, z;
			float scale_x, scale_y, scale_z;
			float axis_x, axis_y, axis_z;

		public: 
			Model3D(std::string path, glm::vec3 modelPos);
			
		public:
			glm::mat4 transform();
			
	};
}

#endif

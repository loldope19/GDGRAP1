#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>


class model3D {
	private:
		float x, y, z;
		float scale_x, scale_y, scale_z;
		float axis_x, axis_y, axis_z;

	private:
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> texCoords;
		std::vector<unsigned int> indices;

	public:
		model3D(std::string filePath);
		~model3D();

	public:
		void loadModel(std::string& filePath);
		void render();

};
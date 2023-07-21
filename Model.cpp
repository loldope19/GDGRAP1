#include "Model.h"

using namespace model;

Model3D::Model3D(std::string path, glm::vec3 modelPos) {
	this->path = path;
	this->x = modelPos.x;
	this->y = modelPos.y;
	this->z = modelPos.z;
	this->scale_x = this->scale_y = this->scale_z = 0.5f;
	this->axis_x = this->axis_y = 1.0f;
	this->axis_z = 0.0f;
}

// transform - Handles the transformation matrix of the model
glm::mat4 Model3D::transform() {
	glm::mat4 identity_matrix = glm::mat4(1.0f);
	glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(this->x, this->y, this->z));
	transformation_matrix = glm::scale(transformation_matrix, glm::vec3(this->scale_x, this->scale_y, this->scale_z));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(0.0f), glm::normalize(glm::vec3(this->axis_x, 0, this->axis_z)));     // Rotation w/ Normalized X-Axis
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.0f), glm::normalize(glm::vec3(0, this->axis_y, this->axis_z)));     // Rotation w/ Normalized Y-Axis

	return transformation_matrix;
}
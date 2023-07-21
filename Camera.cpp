#include "camera.h"

// Default camera values
const float SPEED = 0.01f;
const float SENSITIVITY = 0.3f;

using namespace camera;

Camera::Camera(glm::vec3 position, glm::vec3 up) : center(glm::vec3(1.0f, 0.0f, 15.0f)), fMovementSpeed(SPEED), fSensitivity(SENSITIVITY) {
	this->cameraPos = position;
	this->worldUp = up;
	this->center = center;
	this->fYaw = -90.0f;
	this->fPitch = 0.0f;
	updateCameraVectors();
};

// getViewMatrix - Returns the camera's viewing position and angle as to where it is currently looking
glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(this->cameraPos, this->cameraPos + this->center, this->worldUp);
}

// processKeyboard - Handles keyboard input in conjunction with processInput, but with preference to camera controls
// @param Camera_Movement - an enum variable to help find which arrow key the player pressed
void Camera::processKeyboard(Camera_Movement direction) {
	if (direction == UP) {
		//this->cameraPos += this->center * fMovementSpeed;
	}
	if (direction == DOWN) {
		//this->cameraPos -= this->center * fMovementSpeed;
	}
	if (direction == LEFT) {
		//this->cameraPos -= this->right * fMovementSpeed;
	}
	if (direction == RIGHT) {
		//this->cameraPos += this->right * fMovementSpeed;
	}
	// this->cameraPos.y = 0.0f;		// Remove '//' if you want the camera to stay grounded (i.e: not fly)
}

// processMouseMovement - Handles mouse movement in conjunction with mouse_callback
// @param xOffset - Mouse X-Position's offset
// @param yOffset - Mouse Y-Position's offset
// @param constrainPitch - restricts the camera's Y-rotation so it does not rotate full-circle
void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
	xOffset *= this->fSensitivity;
	yOffset *= this->fSensitivity;

	this->fYaw += xOffset;
	this->fPitch += yOffset;

	if (constrainPitch) {
		if (this->fPitch > 89.0f)
			this->fPitch = 89.0f;
		if (this->fPitch < -89.0f)
			this->fPitch = -89.0f;
	}
	updateCameraVectors();
}

// updateCameraVectors - The main updating function of the camera
void Camera::updateCameraVectors() {
	glm::vec3 front = {};
	front.x = cos(glm::radians(this->fYaw)) * cos(glm::radians(this->fPitch));
	front.y = sin(glm::radians(this->fPitch));
	front.z = sin(glm::radians(this->fYaw)) * cos(glm::radians(this->fPitch));
	center = glm::normalize(front);

	this->right = glm::normalize(glm::cross(this->center, this->worldUp));
	this->Up = glm::normalize(glm::cross(this->right, this->center));
}
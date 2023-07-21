#include "PerspectiveCam.h"

using namespace camera;

PerspectiveCamera::PerspectiveCamera(glm::vec3 position, glm::vec3 up, float fFOV, float fAspect, 
	float fNear, float fFar) : Camera(position, up), fFOV(fFOV), fAspect(fAspect), fNear(fNear),
	fFar(fFar) {}

glm::mat4 PerspectiveCamera::getViewMatrix() {
	return glm::perspective(fFOV, fAspect, fNear, fFar) * glm::lookAt(this->cameraPos, this->cameraPos + this->center, this->worldUp);
}

void PerspectiveCamera::processKeyboard(Camera_Movement direction) {
	Camera::processKeyboard(direction);
}

void PerspectiveCamera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
	Camera::processMouseMovement(xOffset, yOffset, constrainPitch);
}
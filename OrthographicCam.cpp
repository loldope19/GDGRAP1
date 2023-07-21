#include "OrthographicCam.h"

using namespace camera;

OrthographicCamera::OrthographicCamera(glm::vec3 position, glm::vec3 up, float fLeft, float fRight,
	float fBottom, float fTop, float fNear, float fFar) : Camera(position, up), fLeft(fLeft), 
	fRight(fRight), fBottom(fBottom), fTop(fTop), fNear(fNear), fFar(fFar) {}

glm::mat4 OrthographicCamera::getViewMatrix() {
	return glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar) * glm::lookAt(this->cameraPos, this->cameraPos + this->center, this->worldUp);
}

void OrthographicCamera::processKeyboard(Camera_Movement direction) {
	Camera::processKeyboard(direction);
}

void OrthographicCamera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
	Camera::processMouseMovement(xOffset, yOffset, constrainPitch);
}

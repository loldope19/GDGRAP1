#ifndef CAMERA_PERSPECTIVE_H
#define CAMERA_PERSPECTIVE_H

#include "Camera.h"

namespace camera {
	class PerspectiveCamera : public Camera {
		private:
			float fFOV, fAspect, fNear, fFar;

		public:
			PerspectiveCamera(glm::vec3 position, glm::vec3 up, float fFOV, float fAspect, float fNear, float fFar);
			glm::mat4 getViewMatrix() override;
			void processKeyboard(Camera_Movement direction) override;
			void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) override;
	};
}

#endif

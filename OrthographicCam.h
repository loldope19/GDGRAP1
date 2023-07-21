#ifndef CAMERA_ORTHOGRAPHIC_H
#define CAMERA_ORTHOGRAPHIC_H

#include "Camera.h"

namespace camera {
	class OrthographicCamera : public Camera {
		private:
			float fLeft, fRight;
			float fBottom, fTop;
			float fNear, fFar;

		public:
			OrthographicCamera(glm::vec3 position, glm::vec3 up, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);
			
		public:
			glm::mat4 getViewMatrix() override;
			void processKeyboard(Camera_Movement direction) override;
			void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) override;
	};
}

#endif // !CAMERA_ORTHOGRAPHIC_H


#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace camera {
	enum Camera_Movement {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	class Camera {
		public:
			glm::vec3 cameraPos, center, worldUp, right, Up;
			float fYaw, fPitch;
			float fMovementSpeed, fSensitivity;

		public:
			Camera(glm::vec3 position, glm::vec3 up);
			virtual glm::mat4 getViewMatrix();
			virtual void processKeyboard(Camera_Movement direction);
			virtual void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch);

		protected:
			void updateCameraVectors();
        

	};
}
	
#endif
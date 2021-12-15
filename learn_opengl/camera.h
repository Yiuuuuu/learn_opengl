#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

//Define several possible options
enum camera_direction {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Default camera values
const float PITCH = 0.0f;
const float YAW = -90.0f;		//Yaw = -90.0f means facing front
const float CAMERASPEED = 10.0f;
const float SENSIVITY = 0.15f;
const float ZOOM = 45.0f;

class camera {
public:
	//Camera Attributes
	glm::vec3 cameraPos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	//Euler Angles
	float pitch;
	float yaw;
	//Camera Setting
	float movementSpeed;
	float mouseSentivity;
	float zoom;


	//Constructor
	camera(glm::vec3 _cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _upVectorWorld = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = YAW, float _pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(CAMERASPEED), mouseSentivity(SENSIVITY), zoom(ZOOM) {
		cameraPos = _cameraPos;
		worldUp = _upVectorWorld;
		yaw = _yaw;
		pitch = _pitch;
		updateCameraVector();
	}

	camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _pitch = PITCH, float _yaw = YAW) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(CAMERASPEED), mouseSentivity(SENSIVITY), zoom(ZOOM) {
		cameraPos = glm::vec3(_posX, _posY, _posZ);
		worldUp = glm::vec3(_upX, _upY, _upZ);
		yaw = _yaw;
		pitch = _pitch;
		updateCameraVector();
	}

	glm::mat4 viewMatrix() {
		return glm::lookAt(cameraPos, cameraPos + front, up);
	}

	void processKeyboard(camera_direction direction, float deltaTime) {
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			cameraPos += front * velocity;
		if (direction == BACKWARD)
			cameraPos -= front * velocity;
		if (direction == LEFT)
			cameraPos -= right * velocity;
		if (direction == RIGHT)
			cameraPos += right * velocity;
	}

	void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= mouseSentivity;
		yOffset *= mouseSentivity;

		pitch += yOffset;
		yaw	  += xOffset;

		if (constrainPitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
		updateCameraVector();
	}

	void processMouseScroll(float xOffset, float yOffset) {
		if (zoom >= 1.0f && zoom <= 45.0f)
			zoom -= yOffset;
		if (zoom < 1.0f)
			zoom = 1.0f;
		if (zoom > 45.0f)
			zoom = 45.0f;
	}

	void updateCameraVector() {
		glm::vec3 tempFront;
		tempFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		tempFront.y = sin(glm::radians(pitch));
		tempFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front = glm::normalize(tempFront);

		right = glm::normalize(glm::cross(front, worldUp));
		up	  = glm::normalize(glm::cross(right, front));
	}
};


//My lookAt function
glm::mat4 myLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 upVector) {
	glm::vec3 zaxis = glm::normalize(position - target);
	glm::vec3 xaxis = glm::cross(glm::normalize(upVector), zaxis);
	glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0] = glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
	rotation[1] = glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
	rotation[2] = glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0.0f);

	glm::mat4 translation = glm::mat4(1.0f);
	translation[3][0] = -position.x;
	translation[3][1] = -position.y;
	translation[3][2] = -position.z;

	return rotation * translation;
}
#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	// Getters
	glm::vec3 getPosition() const { return position; }
	glm::vec3 getFront() const { return front; }
	glm::vec3 getUp() const { return up; }
	glm::vec3 getRight() const { return right; }
	GLfloat getYaw() const { return yaw; }
	GLfloat getPitch() const { return pitch; }

	// Setters
	void setPosition(const glm::vec3& newPosition) { position = newPosition; }
	void setFront(const glm::vec3& newFront) { front = newFront; }
	void setYaw(GLfloat newYaw) { yaw = newYaw; }
	void setPitch(GLfloat newPitch) { pitch = newPitch; }

	void update();

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

};


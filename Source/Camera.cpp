
#include "Camera.h"
#include "math3d.h"
#include "opengl.h"

Camera::Camera(float yFov, float aspect, float zNear, float zFar)
	: speed(1.0f)
{
	this->cursorResetX = 0;
	this->cursorResetY = 0;
	this->forward = false;
	this->backward = false;
	this->left = false;
	this->right = false;
	this->weightX = 0.1f;
	this->weightY = 0.1f;
	this->rotationX = 90.0f;
	this->rotationY = 0.0f;
	this->yFov = yFov;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	this->position = vec3(0.0f, 0.0f, 0.0f);
	this->viewDirection = vec3(1.0f, 0.0f, 0.0f);
}

void Camera::OnMouseMoved(int ix, int iy) {
	// calculate the delta between the point and the screen center
	int dx = cursorResetX - ix;
	int dy = iy - cursorResetY;

	rotationY += ((float)dx * weightX);
	rotationX += ((float)dy * weightY);

	// clamp x rotation
	if(rotationX < 1.0f) {
		rotationX = 1.0f;
	} else if(rotationX > 179.0f) {
		rotationX = 179.0f;
	}

	// calculate view Dir based on rotations
	viewDirection = CalculateViewDirection(rotationX, rotationY);
}

void Camera::OnKeyPressed(unsigned char c) {
	switch(c) {
	case 'w': forward = true; break;
	case 's': backward = true; break;
	case 'a': left = true; break;
	case 'd': right = true; break;
	}
}

void Camera::OnKeyReleased(unsigned char c) {
	switch(c) {
	case 'w': forward = false; break;
	case 's': backward = false; break;
	case 'a': left = false; break;
	case 'd': right = false; break;
	}
}

void Camera::SetCursorResetPos(int x, int y) {
	cursorResetX = x;
	cursorResetY = y;
}

void Camera::SetAspectRatio(float aspect) {
	this->aspect = aspect;
}

void Camera::Update(float timeDelta) {

	// reset cursor position
	glutWarpPointer(cursorResetX, cursorResetY);

	vec3 vel(0,0,0);
	if(forward) vel += viewDirection * speed;
	if(backward) vel -= viewDirection * speed;

	//vel.normalize();
	static float speed = 100.0f;
	position += vel * timeDelta * speed;
}

void Camera::SetProjectionMatrix() const {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(yFov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}


void Camera::SetViewMatrix() const {
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	vec3 lookAt = position + viewDirection;
	gluLookAt(position.x, position.y, position.z, lookAt.x, lookAt.y, lookAt.z, 0, 1, 0);
}


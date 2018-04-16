#pragma once

#include "math3d.h"

class Camera {
	int	lastX;
	int	lastY;
	int	cursorResetX;
	int	cursorResetY;
	float	weightX;
	float	weightY;
	float	rotationY;
	float	rotationX;
	float	yFov;
	float	aspect;
	float	zNear;
	float	zFar;
	float	speed;
	//Plane3d	viewPlanes[6];
	//Plane3d worldViewPlanes[6];
	vec3	viewDirection;
	vec3	position;
	//void (*SetCursorPosition)(int x, int y);

	// movement flags
	bool	forward;
	bool	backward;
	bool	left;
	bool	right;
public:

	Camera(float yFov, float aspect, float zNear, float zFar);
	void Update(float timeDelta);
	void OnMouseMoved(int x, int y);
	void OnKeyPressed(unsigned char c);
	void OnKeyReleased(unsigned char c);
	void SetViewMatrix() const;
	void SetProjectionMatrix() const;
	void SetCursorResetPos(int x, int y);
	void SetAspectRatio(float aspect);
	void SetSpeed(float speed) { this->speed = speed; }
	float GetSpeed() const { return speed; }
	const vec3& GetPosition() const { return position; }
};

#include "Camera.h"
#include <system/platform.h>
#include <box2d/Box2D.h>

Camera::Camera()
{
	cameraUp.set_value(0.0f, 1.0f, 0.0f);
	cameraRight.Set(1.0f, 0.0f);
	cameraTarget.set_value(8.0f, 8.0f, 0.0f);

	currentTarget = 0;
}

// 1 is clockwise, -1 is anti clockwise
void Camera::RotateCam(int dir = 1)
{
	//BROKEN FIX IT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float camx;
	float camy;
	camx = (cos(-dir * 3.14159 / 40) * cameraUp.x()) - (sin(-dir * 3.14159 / 40) * cameraUp.y());
	camy = (sin(-dir * 3.14159 / 40) * cameraUp.x()) + (cos(-dir * 3.14159 / 40) * cameraUp.y());
	
	if (b2Abs(camx - cameraTarget.x()) < CAMMARGIN || b2Abs(camx - cameraTarget.x()) < CAMMARGIN)
	{
		camx = cameraTarget.x();
	}
	if (b2Abs(camy - cameraTarget.y()) < CAMMARGIN || b2Abs(camy - cameraTarget.y()) < CAMMARGIN)
	{
		camy = cameraTarget.y();
	}

	cameraUp.set_value(camx, camy, cameraUp.z());

	cameraRight.Set(camy, -camx);
}

gef::Vector4 Camera::GetCameraUp()
{
	return cameraUp;
}

b2Vec2 Camera::GetCameraRight()
{
	return cameraRight;
}

bool Camera::GetRotating()
{
	return rotating;
}

void Camera::SetRotating(bool value)
{
	rotating = value;
}

bool Camera::GetRotatePause()
{
	return rotatePause;
}

void Camera::SetRotatePause(bool value)
{
	rotatePause = value;
}

bool Camera::CameraTargetReached()
{
	if (cameraUp.x() == cameraTarget.x() && cameraUp.y() == cameraTarget.y())
	{
		cameraTarget.set_value(0.0f, 0.0f, 0.0f);
		SetRotatePause(false);
		return true;
	}
	else
	{
		return false;
	}
}

void Camera::ChangeCameraTarget(int dir)
{
	currentTarget += dir;
	if (currentTarget > 3)
		currentTarget = 0;
	else if (currentTarget < 0)
		currentTarget = 3;
	SetCameraTarget(currentTarget);
}

void Camera::SetCameraTarget(int dir)
{
	switch (dir)
	{
		case 0:
			cameraTarget.set_value(0.0f, 1.0f, 0.0f);
			break;

		case 1:
			cameraTarget.set_value(1.0f, 0.0f, 0.0f);
			break;

		case 2:
			cameraTarget.set_value(0.0f, -1.0f, 0.0f);
			break;

		case 3:
			cameraTarget.set_value(-1.0f, 0.0f, 0.0f);
			break;

	}

}

Camera::~Camera()
{
}

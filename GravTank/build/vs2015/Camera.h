#pragma once
#include <system/platform.h>
#include <box2d/Box2D.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class Camera
{
public:
	Camera();
	~Camera();
	void RotateCam(int dir);
	gef::Vector4 GetCameraUp();
	b2Vec2 GetCameraRight();
	bool GetRotating();
	void SetRotating(bool value);
	bool GetRotatePause();
	void SetRotatePause(bool value);
	bool CameraTargetReached();
	void ChangeCameraTarget(int dir);
	

private:
	void SetCameraTarget(int dir); // 0 1 2 3 = N E S W

	bool rotatePause = false;
	bool rotating = false;
	gef::Vector4 cameraUp;
	//gef::Vector4 cameraForward;
	b2Vec2 cameraRight;
	gef::Vector4 cameraTarget;

	int currentPos;
	int currentTarget;
	const float CAMMARGIN = 0.11f;
};


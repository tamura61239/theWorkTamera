#pragma once
#include"camera.h"
#include"camera_operation.h"
#include<string>

class CameraManager
{
public:
	CameraManager() {}
	CameraManager(ID3D11Device* device, std::string fileName);
	void Editor();
	void Update(float elapsed_time);
	//setter
	void SetDefaultPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		mCamera->SetPerspective(fov, aspect, nearZ, farZ);

	}
	//getter
	Camera* GetCamera() { return mCamera.get(); }
	CameraOperation* GetCameraOperation() { return mCameraOperation.get(); }
private:
	std::unique_ptr<Camera>mCamera;
	std::unique_ptr<CameraOperation>mCameraOperation;
	float defaultFov = 0;
	float defaultAspect = 0;
	float defaultNearZ = 0;
	float defaultFarZ = 0;
};

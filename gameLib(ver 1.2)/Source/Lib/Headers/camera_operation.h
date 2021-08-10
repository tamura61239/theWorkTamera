#pragma once
#include"camera.h"
#include<memory>

class CameraOperation
{
public:
	//コンストラクタ
	CameraOperation(Camera*camera);
	//エディタ
	void Editor(void*id,float x=0,float y=0);
	//更新
	void Update(Camera* camera, float elapsedTime);
	//デバックカメラ
	void DebugCamera(Camera* camera);
	//カメラの種類
	enum CAMERA_TYPE
	{
		NORMAL,
		DEBUG
	};
	//setter
	void SetCameraType(CAMERA_TYPE type) { mType = type; }
private:
	//エディタ変数
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F mOldCursor;
	VECTOR2F mNewCursor;
	float mDistance;
	VECTOR2F mRotate;
};
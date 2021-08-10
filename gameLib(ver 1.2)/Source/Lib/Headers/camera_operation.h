#pragma once
#include"camera.h"
#include<memory>

class CameraOperation
{
public:
	//�R���X�g���N�^
	CameraOperation(Camera*camera);
	//�G�f�B�^
	void Editor(void*id,float x=0,float y=0);
	//�X�V
	void Update(Camera* camera, float elapsedTime);
	//�f�o�b�N�J����
	void DebugCamera(Camera* camera);
	//�J�����̎��
	enum CAMERA_TYPE
	{
		NORMAL,
		DEBUG
	};
	//setter
	void SetCameraType(CAMERA_TYPE type) { mType = type; }
private:
	//�G�f�B�^�ϐ�
	CAMERA_TYPE mType;
	//DebugCamera
	VECTOR2F mOldCursor;
	VECTOR2F mNewCursor;
	float mDistance;
	VECTOR2F mRotate;
};
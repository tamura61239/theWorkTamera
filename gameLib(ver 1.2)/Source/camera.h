#pragma once
#include"vector.h"

class Camera
{
public:
	Camera();
	void CalculateMatrix();
	//setter
	void SetPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		mFov = fov;
		mAspect = aspect;
		mNearZ = nearZ;
		mFarZ = farZ;
	}
	void SetEye(const VECTOR3F& eye) { mEye = eye; }
	void SetFocus(const VECTOR3F& focus) { mFocus = focus; }
	void SetUp(const VECTOR3F& up) { mUp = up; }
	//getter
	const VECTOR3F& GetEye() { return mEye; }
	const VECTOR3F& GetFocus() { return mFocus; }
	const VECTOR3F& GetUp() { return mUp; }
	const VECTOR3F& GetFront() { return mFront; }
	const VECTOR3F& GetRight() { return mRight; }
	const FLOAT4X4& GetView() { return mView; }
	const FLOAT4X4& GetProjection() { return mProjection; }
private:
	FLOAT4X4 mView;//�r���[�s��
	FLOAT4X4 mProjection;//�v���W�F�N�V�����s��
	VECTOR3F mEye;// ���_ 
	VECTOR3F mFocus;//�����_
	VECTOR3F mUp;//��x�N�g��
	VECTOR3F mFront;//�O�x�N�g��
	VECTOR3F mRight;//�E�x�N�g��
	float mFov;//��p
	float mAspect;//�A�X�y�N�g��
	float mNearZ; // �\���ŋߖʂ܂ł̋���
	float mFarZ;//�\���ŉ��ʂ܂ł̋���
};
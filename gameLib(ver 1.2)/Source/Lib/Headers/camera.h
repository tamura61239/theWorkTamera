#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include"constant_buffer.h"
#include<memory>

class Camera
{
public:
	//�R���X�g���N�^
	Camera(ID3D11Device* device);
	//view projection�s��̌v�Z
	void CalculateMatrix();
	//view projection�s��̌v�Z
	void CalculateParallelMatrix();
	//���̃t���[����view projection�s���GPU�ɑ���
	void NowActive(ID3D11DeviceContext* context, const int number, const bool vs = false, const bool ps = false, const bool gs = false) { mNowFrame->Activate(context, number, vs, ps, gs); }
	void NowDactive(ID3D11DeviceContext* context) { mNowFrame->DeActivate(context); }
	//�O�̃t���[����view projection�s���GPU�ɑ���
	void BeforeActive(ID3D11DeviceContext* context, const int number, const bool vs = false, const bool ps = false, const bool gs = false) { mBeforeFrame->Activate(context, number, vs, ps, gs); }
	void BeforeDactive(ID3D11DeviceContext* context) { mBeforeFrame->DeActivate(context); }
	//setter
	void SetPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		mFov = fov;
		mAspect = aspect;
		mNearZ = nearZ;
		mFarZ = farZ;
	}
	void SetParallelPerspective(float width, float hight, float nearZ, float farZ)
	{
		mWidth = width;
		mHight = hight;
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
	const FLOAT4X4& GetView() { return mNowFrame->data.view; }
	const FLOAT4X4& GetProjection() { return mNowFrame->data.projection; }
	const float GetFov() { return mFov; }
	const float GetAspect() { return mAspect; }
	const float GetNear() { return mNearZ; }
	const float GetFar() { return mFarZ; }
private:
	VECTOR3F mEye;// ���_ 
	VECTOR3F mFocus;//�����_
	VECTOR3F mUp;//��x�N�g��
	VECTOR3F mFront;//�O�x�N�g��
	VECTOR3F mRight;//�E�x�N�g��
	float mFov;//��p
	float mWidth;//����
	float mHight;//�c��
	float mAspect;//�A�X�y�N�g��
	float mNearZ; // �\���ŋߖʂ܂ł̋���
	float mFarZ;//�\���ŉ��ʂ܂ł̋���
	//�萔�o�b�t�@
	struct Cb
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	std::unique_ptr<ConstantBuffer<Cb>>mBeforeFrame;
	std::unique_ptr<ConstantBuffer<Cb>>mNowFrame;
};
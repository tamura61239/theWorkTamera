#pragma once
#include"vector.h"

class Obj3D
{
public:
	Obj3D():mPosition(0,0,0), mAngle(0,0,0), mScale(1,1,1), mWorld(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0){}
	void CalculateTransform()
	{
		DirectX::XMMATRIX W, s, r, t;
		//�X�P�[���s��
		s = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		//��]�s��
		r = DirectX::XMMatrixRotationRollPitchYaw(mAngle.x, mAngle.y, mAngle.z);
		//�ړ��s��
		t = DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		//���[���h�s��
		W = s * r * t;
		DirectX::XMStoreFloat4x4(&mWorld, W);
	}
	//setter
	void SetPosition(const VECTOR3F position) { mPosition = position; }
	void SetAngle(const VECTOR3F angle) { mAngle = angle; }
	void SetScale(const VECTOR3F scale) { mScale = scale; }
	//getter
	const VECTOR3F& GetPosition() { return mPosition; }
	const VECTOR3F& GetAngle() { return mAngle; }
	const VECTOR3F& GetScale() { return mScale; }
	const FLOAT4X4& GetWorld() { return mWorld; }
protected:
	VECTOR3F mPosition;
	VECTOR3F mAngle;
	VECTOR3F mScale;
	FLOAT4X4 mWorld;
};
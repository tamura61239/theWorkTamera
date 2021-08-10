#include "camera.h"
#include<d3d11.h>
#include"misc.h"

Camera::Camera(ID3D11Device* device) :mEye(0, 0, -200.0f), mFocus(0, 0, 0), mUp(0, 1, 0), mFront(0, 0, 1), mRight(1, 0, 0), mFov(0), mAspect(0), mNearZ(0), mFarZ(0), mWidth(0), mHight(0)
{
	//�萔�o�b�t�@�̍쐬
	mBeforeFrame = std::make_unique<ConstantBuffer<Cb>>(device);
	mNowFrame = std::make_unique<ConstantBuffer<Cb>>(device);
}

void Camera::CalculateMatrix()
{
	mBeforeFrame->data.view = mNowFrame->data.view;
	mBeforeFrame->data.projection = mNowFrame->data.projection;
	//�r���[�s��쐬
	DirectX::XMMATRIX V;

	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(mEye.x, mEye.y, mEye.z, 1.0f);//���_
	focus = DirectX::XMVectorSet(mFocus.x, mFocus.y, mFocus.z, 1.0f);//�����_
	up = DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 1.0f);//��x�N�g��

	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	DirectX::XMStoreFloat4x4(&mNowFrame->data.view, V);
	mFront = VECTOR3F(mNowFrame->data.view._31, mNowFrame->data.view._32, mNowFrame->data.view._33);
	//�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX P;

	P = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNearZ, mFarZ);

	DirectX::XMStoreFloat4x4(&mNowFrame->data.projection, P);

}

void Camera::CalculateParallelMatrix()
{
	//�r���[�s��쐬
	DirectX::XMMATRIX V;

	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(mEye.x, mEye.y, mEye.z, 1.0f);//���_
	focus = DirectX::XMVectorSet(mFocus.x, mFocus.y, mFocus.z, 1.0f);//�����_
	up = DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 1.0f);//��x�N�g��

	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	DirectX::XMStoreFloat4x4(&mNowFrame->data.view, V);
	//�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX P;

	P = DirectX::XMMatrixOrthographicLH(mWidth, mHight, mNearZ, mFarZ);

	DirectX::XMStoreFloat4x4(&mNowFrame->data.projection, P);

}


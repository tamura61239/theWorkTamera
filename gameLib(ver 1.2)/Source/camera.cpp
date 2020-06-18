#include "camera.h"
#include<d3d11.h>

Camera::Camera():mEye(0,0,-200.0f),mFocus(0,0,0),mUp(0,1,0),mFront(0,0,1),mRight(1,0,0),mView(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),mFov(0),mAspect(0),mNearZ(0),mFarZ(0)
{
}

void Camera::CalculateMatrix()
{
	//�r���[�s��쐬
	DirectX::XMMATRIX V;
	
	DirectX::XMVECTOR eye, focus, up;
	eye = DirectX::XMVectorSet(mEye.x, mEye.y, mEye.z, 1.0f);//���_
	focus = DirectX::XMVectorSet(mFocus.x, mFocus.y, mFocus.z, 1.0f);//�����_
	up = DirectX::XMVectorSet(mUp.x, mUp.y, mUp.z, 1.0f);//��x�N�g��

	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	DirectX::XMStoreFloat4x4(&mView, V);
	//�v���W�F�N�V�����s��쐬
	DirectX::XMMATRIX P;

	P = DirectX::XMMatrixPerspectiveFovLH(mFov, mAspect, mNearZ, mFarZ);

	DirectX::XMStoreFloat4x4(&mProjection, P);

}
#pragma once
#include"vector.h"
#include<wrl.h>
#include <d3d11.h>
#include"constant_buffer.h"
#include<memory>
//�X�|�b�g���C�g�̃f�[�^
struct SpotLight
{
	float index;
	float range;//���̓͂��͈�
	float type; //�L����������
	float nearArea;
	float farArea;
	float dumy0;
	float dumy1;
	float dumy2;
	VECTOR4F position;
	VECTOR4F color;
	VECTOR4F dir;
};
//�|�C���g���C�g�̃f�[�^
struct PointLight {
	float index;
	float range;//���̓͂��͈�
	float type; //�L����������
	float dumy;
	VECTOR4F position;
	VECTOR4F color;
};
//�|�C���g���C�g��X�|�b�g���C�g�̍ő吔
static const int POINTMAX = 32;
static const int SPOTMAX = 32;
//���C�g�Ǘ��N���X
class Light
{
public:
	//�o�b�t�@�����֐�
	void CreateLightBuffer(ID3D11Device* device);
	//setter
	void SetLightDirection(const VECTOR4F& lightDirection) { mCbDefLight->data.mLightDirection = lightDirection; }
	void SetLightColor(const VECTOR4F& lightColor) { mCbDefLight->data.mLightColor = lightColor; }
	void SetAmbientColor(const VECTOR4F& ambientColor) { mCbDefLight->data.mAmbientColor = ambientColor; }
	void SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range);
	void SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea);
	void ConstanceLightBufferSetShader(ID3D11DeviceContext* context,VECTOR3F eye);
	//getter
	const VECTOR4F& GetLightDirection() { return mCbDefLight->data.mLightDirection; }
	const VECTOR4F& GetLightColor() { return mCbDefLight->data.mLightColor; }
	const VECTOR4F& GetAmbientColor() { return mCbDefLight->data.mAmbientColor; }
	PointLight* GetPointLight() { return mCbLight->data.pointLight; }
	SpotLight* GetSpotLight() { return mCbLight->data.spotLight; }

	//�C���X�^���X�֐�
	static Light& GetInctance()
	{
		static Light light;
		return light;
	}
	//�G�f�B�^
	void Editor();
private:
	//�R���X�g���N�^
	Light();
	//�萔�o�b�t�@
	struct CbLight
	{
		PointLight pointLight[POINTMAX];
		SpotLight spotLight[SPOTMAX];
	};
	struct CbDefLight
	{
		VECTOR4F mLightColor;
		VECTOR4F mLightDirection;
		VECTOR4F mAmbientColor;
		VECTOR4F mEyePosition;
		VECTOR3F mSkyColor;
		float dummy1;
		VECTOR3F mGroundColor;
		float dummy2;
	};
	std::unique_ptr<ConstantBuffer<CbLight>>mCbLight;
	std::unique_ptr<ConstantBuffer<CbDefLight>>mCbDefLight;
};
#define pLight (Light::GetInctance())
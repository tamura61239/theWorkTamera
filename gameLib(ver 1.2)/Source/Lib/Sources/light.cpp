#include "light.h"
#include "misc.h"
#include"camera_manager.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/
/***************************�R���X�g���N�^**************************/

Light::Light()
{
	//ZeroMemory(mPointLight, sizeof(PointLight) * POINTMAX);
	//ZeroMemory(mSpotLight, sizeof(SpotLight) * SPOTMAX);
	//memset(&mDefLight, 1, sizeof(mDefLight));
	//FileFunction::Load(mDefLight, "Data/file/light.bin", "rb");
}
/**************************�o�b�t�@�̐���***************************/

void Light::CreateLightBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	// �萔�o�b�t�@
	{
		mCbDefLight = std::make_unique<ConstantBuffer<CbDefLight>>(device);
		mCbLight = std::make_unique<ConstantBuffer<CbLight>>(device);
	}
	//�t�@�C������f�[�^�̃��[�h
	FileFunction::Load(mCbDefLight->data, "Data/file/light.bin", "rb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void Light::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("light");
	//�p�����[�^�[�𒲐�����
	if (ImGui::CollapsingHeader("default light"))
	{
		float* lightDerection[4] = { &mCbDefLight->data.mLightDirection.x,&mCbDefLight->data.mLightDirection.y ,&mCbDefLight->data.mLightDirection.z ,&mCbDefLight->data.mLightDirection.w };
		ImGui::SliderFloat4("light direction", *lightDerection, -1, 1);
		float* lightColor[4] = { &mCbDefLight->data.mLightColor.x,&mCbDefLight->data.mLightColor.y ,&mCbDefLight->data.mLightColor.z ,&mCbDefLight->data.mLightColor.w };
		ImGui::ColorEdit4("light color", *lightColor);
		float* ambientColor[4] = { &mCbDefLight->data.mAmbientColor.x,&mCbDefLight->data.mAmbientColor.y ,&mCbDefLight->data.mAmbientColor.z ,&mCbDefLight->data.mAmbientColor.w };
		ImGui::ColorEdit4("ambient color", *ambientColor);
	}
	float* skyColor[3] = { &mCbDefLight->data.mSkyColor.x,&mCbDefLight->data.mSkyColor.y ,&mCbDefLight->data.mSkyColor.z };
	ImGui::ColorEdit3("sky color", *skyColor);
	float* groundColor[3] = { &mCbDefLight->data.mGroundColor.x,&mCbDefLight->data.mGroundColor.y ,&mCbDefLight->data.mGroundColor.z  };
	ImGui::ColorEdit3("ground color", *groundColor);
	//�Z�[�u
	if (ImGui::Button("save"))FileFunction::Save(mCbDefLight->data, "Data/file/light.bin", "wb");
	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�Z�b�g�֐�
/*****************************************************/

/**************************�|�C���g���C�g�̐ݒ�***************************/

void Light::SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	mCbLight->data.pointLight[index].index = (float)index;
	mCbLight->data.pointLight[index].range = range;
	mCbLight->data.pointLight[index].type = 1.0f;
	mCbLight->data.pointLight[index].dumy = 0.0f;
	mCbLight->data.pointLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	mCbLight->data.pointLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);

}
/**************************�X�|�b�g���C�g�̐ݒ�***************************/

void Light::SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;

	mCbLight->data.spotLight[index].index = (float)index;
	mCbLight->data.spotLight[index].range = range;
	mCbLight->data.spotLight[index].type = 1.0f;
	mCbLight->data.spotLight[index].nearArea = nearArea;
	mCbLight->data.spotLight[index].farArea = farArea;
	mCbLight->data.spotLight[index].dumy0 = 0.0f;
	mCbLight->data.spotLight[index].dumy1 = 0.0f;
	mCbLight->data.spotLight[index].dumy2 = 0.0f;

	mCbLight->data.spotLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	mCbLight->data.spotLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);
	mCbLight->data.spotLight[index].dir = VECTOR4F(dir.x, dir.y, dir.z, 0);

}
/******************************�萔�o�b�t�@��GPU���ɑ���************************/
void Light::ConstanceLightBufferSetShader(ID3D11DeviceContext* context, VECTOR3F eye)
{
	mCbDefLight->data.mEyePosition = VECTOR4F(eye.x, eye.y, eye.z, .0f);
	mCbDefLight->Activate(context, 4, false, true);
	mCbLight->Activate(context, 3, false, true);
}


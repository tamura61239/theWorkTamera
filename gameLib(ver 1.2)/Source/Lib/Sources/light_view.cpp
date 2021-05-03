#include "light_view.h"
#include"light.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

LightView::LightView(ID3D11Device* device, std::string fileName)
{
	//�J�����̐���(���C�g���_��)
	mLightCamera = std::make_unique<Camera>(device);
	std::string filePas = "Data/file/" + fileName + ".bin";
	//�f�[�^���t�@�C�����烍�[�h
	FileFunction::Load(mData, filePas.c_str(), "rb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void LightView::Update(const VECTOR3F& targetPosition, ID3D11DeviceContext* context)
{
	//viewport�擾
	D3D11_VIEWPORT shadowMapViewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &shadowMapViewport);
	//�J�����͈̔͂�ݒ�
	mLightCamera->SetParallelPerspective(shadowMapViewport.Width / mData.viewportMagnification, shadowMapViewport.Height / mData.viewportMagnification, 0.1f, 3000);
	//�����_��ݒ�
	mLightCamera->SetFocus(targetPosition + VECTOR3F(mData.localFocus[0], mData.localFocus[1], mData.localFocus[2]));
	//���C�g�x�N�g�����擾
	VECTOR3F lightDir = VECTOR3F(pLight.GetLightDirection().x, pLight.GetLightDirection().y, pLight.GetLightDirection().z);
	//�J�������W(eye)��ݒ�
	mLightCamera->SetEye(mLightCamera->GetFocus() + lightDir * mData.lightEyeLength);
	//�s��v�Z
	mLightCamera->CalculateParallelMatrix();
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void LightView::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("light view data");
	//eye�̒����_����̋���
	ImGui::InputFloat("light eye length", &mData.lightEyeLength, 10);
	//�J�����͈͂̃p�����[�^�[��ݒ�
	ImGui::InputFloat("viewport magnification", &mData.viewportMagnification, 0.1f);
	//�����_������Ă������W����ǂꂾ�����炷�������߂�
	ImGui::DragFloat3("local focus", mData.localFocus, 1);
	//�Z�[�u����t�@�C���̖��O�����
	ImGui::InputText("file name", mFileName, 256);
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		//�t�@�C���p�X�����߂�
		std::string fName = mFileName;
		std::string filePas = "Data/file/" + fName + ".bin";

		FileFunction::Save(mData, filePas.c_str(), "wb");
		//������
		strcpy_s(mFileName, "");
	}
	ImGui::End();
#endif
}



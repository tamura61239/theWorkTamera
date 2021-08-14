#include "..\Headers\particle_create_point.h"
#include"shader.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

ParticleCreatePoint::ParticleCreatePoint(ID3D11Device* device)/*:*//*mTotalCreateCount(0)*/
{
	mCreateBuffer = std::make_unique<ConstantBuffer<VECTOR4F>>(device);
	HRESULT hr = S_OK;
	//�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/point_create_particle.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//mFilePas = "";
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�t�@�C�����[�h�֐�
/*****************************************************/

void ParticleCreatePoint::Load(std::string filePas)
{
	//mFilePas = filePas;
	FileFunction::Load(mEditData, filePas.c_str(), "rb");
	//memcpy(&mCreateBuffer->data.point, mEditData.point, sizeof(Point) * mEditData.pointCount);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void ParticleCreatePoint::Editor(void* id)
{
	return;
#ifdef USE_IMGUI
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 200));
	ImGui::DragInt("create count", &mEditData.createCount);
	ImGui::SliderInt(u8"�����ꏊ�̐�", & mEditData.pointCount, 0, 15);
	for (int i = 0; i < mEditData.pointCount; i++)
	{
		if (ImGui::TreeNode(("point" + std::to_string(i)).c_str()))
		{
			ImGui::DragFloat3(("position" + std::to_string(i)).c_str(), &mEditData.point[i].position.x);
			ImGui::Checkbox((u8"������ɂ���" + std::to_string(i)).c_str(), &mEditData.point[i].directionSpecifyFlag);
			ImGui::DragFloat3((u8"���o����" + std::to_string(i)).c_str(), &mEditData.point[i].releaseDirection.x);
			ImGui::SliderFloat(("position" + std::to_string(i)).c_str(), &mEditData.point[i].angleToFine, -3.14f, 3.14f);
		}
	}

	ImGui::EndChild();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�p�[�e�B�N���̐����֐�
/*****************************************************/

void ParticleCreatePoint::CreateParticle(ID3D11DeviceContext* context, float elapsdTime)
{
	return;
	//�������𐶐�
	mTotalCreateCount += static_cast<float>(mEditData.createCount) * elapsdTime;
	if (mTotalCreateCount < 1.0f)return;
	//�萔�o�b�t�@�̍X�V
	//mCreateBuffer->data.createCount = static_cast<int>(mTotalCreateCount);
	//�V�F�[�_�[�ƒ萔�o�b�t�@��GPU�ɃZ�b�g
	context->CSSetShader(mShader.Get(), nullptr, 0);
	mCreateBuffer->Activate(context, false, false, false, true);
	
	//context->Dispatch(mCreateBuffer->data.createCount * mEditData.pointCount, 1, 1);
	//GPU�ɃZ�b�g�����f�[�^�����
	mCreateBuffer->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);
}

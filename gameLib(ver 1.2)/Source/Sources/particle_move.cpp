#include "particle_move.h"
#include"shader.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

ParticleMove::ParticleMove(ID3D11Device* device)
{
	mCBuffer = std::make_unique<ConstantBuffer<MoveData>>(device);
	HRESULT hr = S_OK;
	//�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "basic_move";
	mFilePas = "";
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�t�@�C���֐�
/*****************************************************/

/**************************���[�h�֐�***************************/

void ParticleMove::Load(std::string filePas)
{
	FileFunction::Load(mCBuffer->data, filePas.c_str(), "rb");
	mFilePas = filePas;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void ParticleMove::Editor(void* id)
{
#ifdef USE_IMGUI
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 200), ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(mName.c_str());
	ImGui::ColorEdit4("end color", &mCBuffer->data.endColor.x);
	ImGui::DragFloat("timer", &mCBuffer->data.colorChangeTime);
	ImGui::DragFloat("maxSpeed", &mCBuffer->data.maxSpeed);
	ImGui::EndChild();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�p�[�e�B�N���̓����֐�
/*****************************************************/

void ParticleMove::Move(ID3D11DeviceContext* context, UINT x)
{
	return;
	mCBuffer->Activate(context, 2, false, false, false, true);
	context->CSSetShader(mShader.Get(), nullptr, 0);

	context->Dispatch(x, 1, 1);

	mCBuffer->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);

}


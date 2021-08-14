#include "..\Headers\color_anim.h"
#include"file_function.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

ColorAnim::ColorAnim(ID3D11Device* device)
{
	mCBuffer = std::make_unique<ConstantBuffer<ColorData>>(device);
	HRESULT hr = S_OK;
	//�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "color_anim";
	mFilePas = "";

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�t�@�C���֐�
/*****************************************************/

/**************************���[�h�֐�***************************/

void ColorAnim::Load(std::string filePas)
{
	FileFunction::Load(mCBuffer->data, filePas.c_str(), "rb");
	mFilePas = filePas;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void ColorAnim::Editor(void* id)
{
#ifdef USE_IMGUI
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 500), ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(mName.c_str());
	for (int i = 0; i < 4; i++)
	{
		ImGui::ColorEdit4(("color" + std::to_string(i)).c_str(), &mCBuffer->data.color[i].x);
		ImGui::DragFloat(("time" + std::to_string(i)).c_str(), &mCBuffer->data.time[i]);
	}
	ImGui::EndChild();
#endif

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�p�[�e�B�N���̓����֐�
/*****************************************************/

void ColorAnim::Move(ID3D11DeviceContext* context, UINT x)
{
	return;
	context->CSSetShader(mShader.Get(), nullptr, 0);
	mCBuffer->Activate(context, false, false, false, true);
	context->Dispatch(x, 1, 1);
	mCBuffer->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);
}

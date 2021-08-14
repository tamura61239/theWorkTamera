#include "particle_move.h"
#include"shader.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

ParticleMove::ParticleMove(ID3D11Device* device)
{
	mCBuffer = std::make_unique<ConstantBuffer<MoveData>>(device);
	HRESULT hr = S_OK;
	//シェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "basic_move";
	mFilePas = "";
}
/*****************************************************/
//　　　　　　　　　　ファイル関数
/*****************************************************/

/**************************ロード関数***************************/

void ParticleMove::Load(std::string filePas)
{
	FileFunction::Load(mCBuffer->data, filePas.c_str(), "rb");
	mFilePas = filePas;
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
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
//　　　　　　　　　　パーティクルの動き関数
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


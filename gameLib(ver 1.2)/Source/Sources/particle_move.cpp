#include "particle_move.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

ParticleMove::ParticleMove(ID3D11Device* device, std::string fileName)
{
	mCBuffer = std::make_unique<ConstantBuffer<MoveData>>(device);
	HRESULT hr = S_OK;
	//シェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "basic_move";
	mFileName = fileName;
}

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

void ParticleMove::Move(ID3D11DeviceContext* context, UINT x)
{
	mCBuffer->Activate(context, 2, false, false, false, true);
	context->CSSetShader(mShader.Get(), nullptr, 0);

	context->Dispatch(x, 1, 1);

	mCBuffer->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);

}


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
	hr = CreateCSFromCso(device, "Data\shader\particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "basic_move";
	mFileName = fileName;
}

void ParticleMove::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin(mName.c_str());
	float* endColor[4] = { &mCBuffer->data.endColor.x,&mCBuffer->data.endColor.y ,&mCBuffer->data.endColor.z ,&mCBuffer->data.endColor.w };
	ImGui::ColorEdit4("end color", *endColor);
	ImGui::DragFloat("timer", &mCBuffer->data.colorChangeTime);
	ImGui::DragFloat("maxSpeed", &mCBuffer->data.maxSpeed);
	ImGui::End();
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


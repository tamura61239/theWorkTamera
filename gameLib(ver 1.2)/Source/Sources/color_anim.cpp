#include "..\Headers\color_anim.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

ColorAnim::ColorAnim(ID3D11Device* device, std::string fileName)
{
	mCBuffer = std::make_unique<ConstantBuffer<ColorData>>(device);
	HRESULT hr = S_OK;
	//シェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/particle_basic_move.cso", mShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = "color_anim";
	mFileName = fileName;

}

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

void ColorAnim::Move(ID3D11DeviceContext* context, UINT x)
{
}

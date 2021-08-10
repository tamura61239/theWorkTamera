#include "scene_particle_editor.h"
#include"light.h"
#include"screen_size.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
SceneParticleEditor::SceneParticleEditor()
{
}

void SceneParticleEditor::Initialize(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			mCameraManager = std::make_unique<CameraManager>(device,"");
			pLight.CreateLightBuffer(device);
			mParticleManager = std::make_unique<GpuParticleManager>(device);
			mObj = std::make_unique<StaticObj>(device, "Data/FBX/source/Dragon_ver2.fbx");
			mRender = std::make_unique<MeshRender>(device);
			mFrames.push_back(std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM));
			mFrames.push_back(std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM));
			mSprite = std::make_unique<Sprite>(device);
			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
			mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));
			mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
			mSampler.push_back(std::make_unique<SamplerState>(device));
			mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS_EQUAL));
			mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP));
			mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);
			mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);

		}, device);
	mEditorFlag = false;

}
static float d = 1;
void SceneParticleEditor::Editor()
{
	if (!mEditorFlag)
	{
		return;
	}

#ifdef USE_IMGUI

	ImGui::Begin("ParticleEditorScene");
	const char* names[EditorName::MAX] = { "camera","light","bloom","particle" };
	for (int i = 0; i < EditorName::MAX; i++)
	{
		ImGui::Selectable(names[i], &mSelectEditors[i]);
	}
	ImGui::InputFloat("d", &d);
	ImGui::End();
	for (int i = 0; i < EditorName::MAX; i++)
	{
		if (mSelectEditors[i])OpenEditor(i);
	}
#endif
}

void SceneParticleEditor::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	elapsed_time *= d;
	mEditorFlag = true;
	mCameraManager->Update(elapsed_time);
	mParticleManager->Update(elapsed_time);
	mObj->CalculateTransform();
}

void SceneParticleEditor::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (!mEditorFlag)
	{
		return;
	}

	mRasterizer->Activate(context);
	mDepth->Activate(context);
	mSampler[0]->Activate(context, 0, false, true);
	mBlend[1]->activate(context);

	pLight.ConstanceLightBufferSetShader(context, mCameraManager->GetCamera()->GetEye());

	mCameraManager->GetCamera()->NowActive(context, 0, true, true, true);
	mFrames[0]->Clear(context);
	mFrames[0]->Activate(context);
	mParticleManager->Render(context);
	//mRender->Render(context, mObj->GetMesh(), mObj->GetWorld());
	mFrames[0]->Deactivate(context);
	mBlend[1]->deactivate(context);

	//mFrames[0]->Clear(context);
	mCameraManager->GetCamera()->NowDactive(context);
	mBlend[0]->activate(context);

	mBloom->BlurTexture(context, mFrames[0]->GetRenderTargetShaderResourceView().Get());
	mFrames[1]->Clear(context);
	mFrames[1]->Activate(context);
	mBloom->Render(context, mFrames[0]->GetRenderTargetShaderResourceView().Get());
	mFrames[1]->Deactivate(context);

	mSprite->Render(context, mFrames[1]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(SCREEN_WIDTH, SCREEN_HEIGHT), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBlend[0]->deactivate(context);
	mRasterizer->DeActivate(context);
	mDepth->DeActive(context);
	mSampler[0]->DeActivate(context);

}

void SceneParticleEditor::Relese()
{
}

void SceneParticleEditor::OpenEditor(const int number)
{
	switch (number)
	{
	case EditorName::CAMERA:
		mCameraManager->Editor();
		break;
	case EditorName::LIGHT:
		pLight.Editor();
		break;
	case EditorName::BLOOM:
		mBloom->Editor();
		break;
	case EditorName::PARTICLE:
		mParticleManager->Editor();
		break;
	}
}

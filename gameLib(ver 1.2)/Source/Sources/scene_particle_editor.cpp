#include "scene_particle_editor.h"
#include"light.h"
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
		}, device);
	mEditorFlag = false;
}

void SceneParticleEditor::Editor()
{
	if (IsNowLoading())
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
	mEditorFlag = true;
	//mParticleManager->Update(elapsed_time);
}

void SceneParticleEditor::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}

	//mRasterizer->Activate(context);
	//mDepth->Activate(context);
	//mSampler[0]->Activate(context, 0, false, true);
	//mBlend[0]->activate(context);

	//mFrames[0]->Clear(context);

	//mBlend[0]->deactivate(context);
	//mRasterizer->DeActivate(context);
	//mDepth->DeActive(context);
	//mSampler[0]->DeActivate(context);

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

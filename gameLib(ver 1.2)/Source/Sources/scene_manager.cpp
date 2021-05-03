#include "scene_manager.h"
#include"scene_particle_editor.h"
#include"key_board.h"

void SceneManager::Initialize(ID3D11Device* device)
{
	mNextScene.reset(nullptr);
	mScene = std::make_unique<SceneParticleEditor>();
	mScene->Initialize(device);
}

void SceneManager::Update(float elapsed_time, ID3D11Device* device)
{
	if (mNextScene.get() != nullptr)
	{
		if (mScene != nullptr)mScene->Relese();
		mScene.reset();
		mNextScene->Initialize(device);
		mScene = std::move(mNextScene);
	}

	pKeyBoad.Update();
	mScene->Editor();
	mScene->Update(elapsed_time);
}

void SceneManager::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	mScene->Render(context, elapsed_time);
}

void SceneManager::ChangeScene(Scene* scene)
{
	mNextScene.reset(scene);
}


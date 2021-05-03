#pragma once
#include<memory>
#include"scene.h"

class SceneManager
{
public:
	void Initialize(ID3D11Device* device);
	void Update(float elapsed_time,ID3D11Device*device);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void ChangeScene(Scene* scene);
	static SceneManager& GetInctance()
	{
		static SceneManager manager;
		return manager;
	}
private:
	SceneManager(){}
	std::unique_ptr<Scene>mScene;
	std::unique_ptr<Scene>mNextScene;
};
#define pSceneManager (SceneManager::GetInctance())
enum SCENETYPE
{
	TITLE,
	GAME,
	OVER,
	CLEAR
};

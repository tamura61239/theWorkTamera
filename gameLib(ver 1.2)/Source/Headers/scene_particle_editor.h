#pragma once
#include"scene.h"
#include"gpu_particle_manager.h"
#include"framebuffer.h"
#include"sprite.h"
#include"bloom.h"
#include"camera_manager.h"


class SceneParticleEditor :public Scene
{
public:
	SceneParticleEditor();
	void Initialize(ID3D11Device* device);
	void Editor();
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void Relese();
private:
	std::unique_ptr<GpuParticleManager>mParticleManager;
	std::vector<std::unique_ptr<FrameBuffer>>mFrames;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<Sprite>mSprite;
	std::unique_ptr<CameraManager>mCameraManager;
	enum EditorName
	{
		CAMERA, LIGHT, BLOOM, PARTICLE, MAX
	};
	bool mSelectEditors[EditorName::MAX];
	void OpenEditor(const int number);
};
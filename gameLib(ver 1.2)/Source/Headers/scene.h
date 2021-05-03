#pragma once
#include<d3d11.h>
#include <thread>
#include <mutex>
#include"Easing.h"
#include"render_state.h"
#include"sampler_state.h"
#include"blend_state.h"
#include<vector>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void Initialize(ID3D11Device* device) = 0;
	virtual void Editor() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void Render(ID3D11DeviceContext* context, float elapsed_time) = 0;
	virtual void Relese() = 0;
protected:
	//Now Loading
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;


	bool IsNowLoading()
	{
		if (loading_thread && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	void EndLoading()
	{
		if (loading_thread && loading_thread->joinable())
		{
			loading_thread->join();
		}
	}
	float mLoadTimer = 0.f;
	//NowLoading�̕\���̕s�����x�v�Z
	VECTOR4F LoadColor(float elapsdTime, float maxTimer = 2)
	{
		mLoadTimer += elapsdTime;
		VECTOR4F color = VECTOR4F(1, 1, 1, 0);
		if (mLoadTimer < maxTimer)
		{
			color.w = Easing::InOutCubic(mLoadTimer, maxTimer);
			return color;
		}
		if (mLoadTimer < maxTimer * 2.f)
		{
			color.w = Easing::InOutCubic(mLoadTimer, maxTimer, 0.f, 1.f);
			return color;
		}
		mLoadTimer = 0;
		return color;
	}
	//�G�f�B�^�֐��p�̕ϐ�
	int mEditorNo = 0;
	bool mEditorFlag = false;
	float mElapsdTimeSpeed = 1.0f;
	bool mStopTime = false;
	bool mScreenShot = false;
	//�X�N���[���V���b�g�Ɏʂ����ǂ���
	std::vector<bool>mPhotographTargets;
	int mTextureNo = 0;
	//�t�F�[�h�C���A�E�g�p�ϐ�
	//�`��ɕK�v��state�ϐ�
	std::unique_ptr<DepthStencilState>mDepth;
	std::unique_ptr<RasterizerState>mRasterizer;
	std::vector<std::unique_ptr<SamplerState>>mSampler;
	std::vector<std::unique_ptr<BlendState>>mBlend;

};
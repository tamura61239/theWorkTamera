#pragma once
#include"gpu_particle_test.h"
#include<vector>
#include<memory>
#include<string>
#include"constant_buffer.h"
#include"drow_shader.h"
#include"particle_editor.h"
#include"particle_base.h"

class GpuParticleManager
{
public:
	GpuParticleManager(ID3D11Device* device);
	void Update(float elapsd_time);
	void Render(ID3D11DeviceContext* context);
#if (PARTICLE_SYSTEM_TYPE==0)
	~GpuParticleManager() { particles.clear(); }
private:

	class GpuParticleObj
	{

	public:
		GpuParticleObj(std::shared_ptr<GpuParticleTest>particle, std::string name)
		{
			mParticle = particle;
			mParticleName = name;
		}
		std::shared_ptr<GpuParticleTest>GetParticle() { return mParticle; }
		std::string GetParticleName() { return mParticleName; }
	private:
		std::shared_ptr<GpuParticleTest>mParticle;
		std::string mParticleName;
	};
	std::vector<GpuParticleObj>particles;
	int particleNimber;
#elif (PARTICLE_SYSTEM_TYPE==1)
	void Editor();
	//パーティクルエディタ関連関数
	void CreateParticle( int type);
	void DeleteParticle(int index)
	{
		if (mParticles.size() <= 0)return;
		assert(index <= mParticles.size());
		mParticles.erase(mParticles.begin() + index);
	}
	~GpuParticleManager()
	{
		mParticles.clear();
	}
private:
	std::unique_ptr<ParticleEditor>mEditor;
	void PSSetTexture(ID3D11DeviceContext* context, int drawType,int texNo);
	std::vector<std::unique_ptr<ParticleBase>>mParticles;
	//std::unique_ptr<TitleParticle>mTitle;
	std::vector<std::string>mTypeNameList;
	//void CreateParticle(std::string name,int type);
	enum DrowType
	{
		Point,Board,Tex,Max
	};
	std::vector<std::string> mDrowTypeNames;
	struct CbTimer
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	std::unique_ptr<ConstantBuffer<CbTimer>>mCbTimer;
	std::unique_ptr<DrowShader>mShaders[DrowType::Max];
	std::vector< Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mTexs;
#endif


};
static const char* drowObjType[] = { "" };

#pragma once
#include"gpu_particle_test.h"
#include<vector>
#include<memory>
#include<string>
#include"constant_buffer.h"

class GpuParticleManager
{
public:
	GpuParticleManager(ID3D11Device* device);
	void Update(float elapsd_time);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
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
private:
	std::vector<std::unique_ptr<GpuParticle>>mParticles;
	int mParticleNo;
	bool mEditorFlag;
	bool mCreateFlag;
	struct CreateEditorData
	{
		std::string name;
		int createType;
		void Reset()
		{
			name = "";
			createType = -1;
		}
	};
	CreateEditorData mCreateEditorData;
	std::vector<std::string>mTypeNameList;
	void CreateParticle(std::string name,int type);
#endif


};
#pragma once
#include"gpu_particle_test.h"
#include<vector>
#include<memory>
#include<string>

class GpuParticleManager
{
public:
	GpuParticleManager(ID3D11Device* device);
	void Update(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
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
};
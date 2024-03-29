#pragma once
#include"gpu_particle_test.h"
#if (PARTICLE_SYSTEM_TYPE==0)

class GpuAbsorptionParticle :public GpuParticleTest
{
public:
	GpuAbsorptionParticle(ID3D11Device* device, int maxParticle = 10000,int size=10, VECTOR3F center = VECTOR3F(0, 0, 0));
	void Update(ID3D11DeviceContext* context, float elapsd_time);
private:
	struct ComputeConstance
	{
		VECTOR3F centerPosition;
		float size;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mConputeConstanceBuffer;
	ComputeConstance mComputeConstance;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mResetUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mResetBuffer;
};
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
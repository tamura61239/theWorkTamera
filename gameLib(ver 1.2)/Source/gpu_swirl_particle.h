#pragma once
#include"gpu_particle_test.h"

class GpuSwirlParticle :public GpuParticleTest
{
public:
	GpuSwirlParticle(ID3D11Device* device, int maxParticle = 10000);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	//void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	struct ComputeConstance
	{
		VECTOR3F centerPosition;
		float dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mConputeConstanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mResetBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mResetUAV;
	ComputeConstance mComputeConstance;
};
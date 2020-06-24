#pragma once
#include"gpu_particle_test.h"

class Gpu2DTextureParticle :public GpuParticleTest
{
public:
	Gpu2DTextureParticle(ID3D11Device* device, VECTOR2F size = VECTOR2F(100, 100));
	void Update(ID3D11DeviceContext* context);
private:
	VECTOR2F mUV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mComputeSRV;
};
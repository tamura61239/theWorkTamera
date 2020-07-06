#pragma once
#include"gpu_particle_test.h"

class GpuCurlNoiseParticle :public GpuParticleTest
{
public:
	GpuCurlNoiseParticle(ID3D11Device* device, const int maxParticle = 100000);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
private:
	struct VertexCurlNoise
	{
		VECTOR4F position;
		VECTOR3F velocity;
		VECTOR4F color;
		float life;
	};
	struct CurlNoise
	{
		VECTOR3F startPosition;
		float timer;
		VECTOR4F startColor;
		VECTOR4F endColor;
	};
	CurlNoise mCurlNoise;
};
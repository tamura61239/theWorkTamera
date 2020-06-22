#pragma once
#include"gpu_particle_test.h"

class GpuCubeParticle :public GpuParticleTest
{
public:
	GpuCubeParticle(ID3D11Device* device, const int maxParticle = 100000, const float size = 2);
};
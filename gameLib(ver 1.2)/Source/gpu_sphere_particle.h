#pragma once
#include"gpu_particle_test.h"

class GpuSphereParticle :public GpuParticleTest
{
public:
	GpuSphereParticle(ID3D11Device* device, const int maxParticle = 100000, const float size = 10);
};
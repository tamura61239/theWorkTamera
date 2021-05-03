#pragma once
#include"gpu_particle_test.h"

#if (PARTICLE_SYSTEM_TYPE==0)

class GpuSphereParticle :public GpuParticleTest
{
public:
	GpuSphereParticle(ID3D11Device* device, const int maxParticle = 100000, const float size = 10);
};
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
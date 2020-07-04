#include "gpu_particle_manager.h"
#include"gpu_cube_particle.h"
#include"gpu_sphere_particle.h"
#include"gpu_swirl_particle.h"
#include"gpu_absorption_particle.h"
#include"gpu_cloud_particle.h"
#include"gpu_wing_particle.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif

GpuParticleManager::GpuParticleManager(ID3D11Device* device):particleNimber(0)
{
	particles.emplace_back(std::make_shared<GpuCubeParticle>(device, 20000), "Cube");
	particles.emplace_back(std::make_shared<GpuSphereParticle>(device, 20000), "Sphere");
	particles.emplace_back(std::make_shared<GpuSwirlParticle>(device, 50000), "Swirl");
	particles.emplace_back(std::make_shared<GpuAbsorptionParticle>(device, 100000), "Absorption");
	particles.emplace_back(std::make_shared<GpuCloudParticle>(device, 200000), "Cloud");
	particles.emplace_back(std::make_shared<GpuWingParticle>(device, 100000), "Wing");
}
static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

void GpuParticleManager::Update(ID3D11DeviceContext* context)
{
	std::vector<std::string>particleName;
	for (auto& particle : particles)
	{
		particleName.push_back(particle.GetParticleName());
	}
#ifdef USE_IMGUI
	ImGui::Begin("particle");
	ImGui::Combo("particleName", &particleNimber, vector_getter, static_cast<void*>(&particleName), particleName.size());
	ImGui::End();
#endif
	particles[particleNimber].GetParticle()->Update(context);
}

void GpuParticleManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	particles[particleNimber].GetParticle()->Render(context, view, projection);
}

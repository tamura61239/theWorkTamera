#include "gpu_particle_manager.h"
#include"gpu_cube_particle.h"
#include"gpu_sphere_particle.h"
#include"gpu_swirl_particle.h"
#include"gpu_absorption_particle.h"
#include"gpu_cloud_particle.h"
#include"gpu_wing_particle.h"
#include"gpu_curl_noise_particle.h"
#include"gpu_noise_geometric_particle.h"
#include"gpu_static_mesh_particle.h"
#include"gpu_skinned_mesh_particle.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include"vector_combo.h"
#endif

GpuParticleManager::GpuParticleManager(ID3D11Device* device):particleNimber(0)
{
	particles.emplace_back(std::make_shared<GpuCubeParticle>(device, 20000), "Cube");
	particles.emplace_back(std::make_shared<GpuSphereParticle>(device, 20000), "Sphere");
	particles.emplace_back(std::make_shared<GpuSwirlParticle>(device, 50000), "Swirl");
	particles.emplace_back(std::make_shared<GpuAbsorptionParticle>(device, 100000), "Absorption");
	particles.emplace_back(std::make_shared<GpuCloudParticle>(device, 200000), "Cloud");
	particles.emplace_back(std::make_shared<GpuWingParticle>(device, 100000), "Wing");
	particles.emplace_back(std::make_shared<GpuCurlNoiseParticle>(device, 100000), "CurlNoise");
	particles.emplace_back(std::make_shared<GpuNoiseGeometricParticle>(device), "NoiseGeometric");
	particles.emplace_back(std::make_shared<GpuStaticMeshParticle>(device, "Data/fbx/source/Dragon_ver2.fbx"), "StaticMesh");
	particles.emplace_back(std::make_shared<GpuSkinnedMeshParticle>(device,"Data/fbx/anim_data.fbx"), "SkinnedMesh");
}

void GpuParticleManager::Update(ID3D11DeviceContext* context, float elapsd_time)
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
	particles[particleNimber].GetParticle()->Update(context, elapsd_time);
}

void GpuParticleManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	particles[particleNimber].GetParticle()->Render(context, view, projection);
}

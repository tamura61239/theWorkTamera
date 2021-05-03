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
#include"framework.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include"vector_combo.h"
#endif
#if (PARTICLE_SYSTEM_TYPE==0)

GpuParticleManager::GpuParticleManager(ID3D11Device* device)
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

void GpuParticleManager::Update(float elapsd_time)
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
#elif (PARTICLE_SYSTEM_TYPE==1)
GpuParticleManager::GpuParticleManager(ID3D11Device* device):mParticleNo(0), mEditorFlag(false), mCreateFlag(false)
{
	mCreateEditorData.Reset();
}

void GpuParticleManager::Update(float elapsd_time)
{
	ID3D11DeviceContext* context = Framework::Instance().GetDeviceContext().Get();
	for (auto& particle : mParticles)
	{
		particle->Update(context);
	}
}

void GpuParticleManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	for (auto& particle : mParticles)
	{
		particle->Render(context);
	}

}
void GpuParticleManager::Editor()
{
#ifdef USE_IMGUI
	std::vector<std::string>nameList;
	for (auto& particle : mParticles)
	{
		nameList.push_back(particle->GetName());
	}
	ImGui::Begin("particleEditor");
	ImGui::Combo("particleName", &mParticleNo, vector_getter, static_cast<void*>(&nameList), nameList.size());
	ImGui::Checkbox("create", &mCreateFlag);
	if (mCreateFlag)
	{
		char* name = new char[mCreateEditorData.name.size() + 1];
		std::char_traits<char>::copy(name, mCreateEditorData.name.c_str(), mCreateEditorData.name.size() + 1);
		ImGui::InputText("name", name, IM_ARRAYSIZE(name));
		mCreateEditorData.name = name;
		delete[] name;
		ImGui::Combo("particleType", &mCreateEditorData.createType, vector_getter, static_cast<void*>(&mTypeNameList), mTypeNameList.size());
		if (mCreateEditorData.name != "" && mCreateEditorData.createType >= 0)
		{
			if (ImGui::Button("create particle"))
			{
				CreateParticle(mCreateEditorData.name, mCreateEditorData.createType);
				mCreateEditorData.Reset();
				mCreateFlag = false;
			}
		}
	}
	else
	{

		ImGui::Checkbox("editor", &mEditorFlag);
		bool deleteFlag = false;
		if (ImGui::Button("delete"))
		{
			deleteFlag = true;
			mParticles.erase(mParticles.begin() + mParticleNo);
		}
		ImGui::End();
		if (!mEditorFlag || deleteFlag)return;
		mParticles[mParticleNo]->Editor();

	}
#endif
}

void GpuParticleManager::CreateParticle(std::string name , int type)
{
	switch (type)
	{
	case 0:
		mParticles.push_back(std::make_unique<GpuParticle>(Framework::Instance().GetDevice().Get(), name));
		break;
	}
}

#endif

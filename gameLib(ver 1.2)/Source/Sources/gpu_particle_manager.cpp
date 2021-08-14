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
#include"texture.h"
#include <stdio.h>
#define STR(var) #var   //引数にした変数を変数名を示す文字列リテラルとして返すマクロ関数
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
	particles.emplace_back(std::make_shared<GpuSkinnedMeshParticle>(device, "Data/fbx/anim_data.fbx"), "SkinnedMesh");
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
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/

GpuParticleManager::GpuParticleManager(ID3D11Device* device) /*:mParticleNo(-1), mEditorFlag(false), mCreateFlag(false)*/
{
	//シェーダー
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},

	};
	mShaders[DrowType::Point] = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "", "Data/shader/particle_render_point_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	mShaders[DrowType::Board] = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	mShaders[DrowType::Tex] = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	//シェーダーの名前リスト
	mDrowTypeNames.push_back(STR(Point));
	mDrowTypeNames.push_back(STR(Board));
	mDrowTypeNames.push_back(STR(Tex));
	//定数バッファ
	mCbTimer = std::make_unique<ConstantBuffer<CbTimer>>(device);
	//生成用データのリセット
	//mCreateEditorData.Reset();
	//生成方法を設定
	mTypeNameList.push_back("one point");
	//描画に使うテクスチャ生成
	wchar_t* names[] =
	{
		L"Data/image/○.png",
		L"",
		L"Data/image/無題1.png",
		L"Data/image/無題2.png",
		L"Data/image/無題3.png",
		L"Data/image/無題4.png",
		L"Data/image/無題5.png",
		L"Data/image/無題6.png",
		L"Data/image/無題7.png",
		L"Data/image/無題8.png",
	};
	HRESULT hr;
	for (auto& name : names)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
		if (wcscmp(name, L"") == 0)
		{
			hr = MakeDummyTexture(device, srv.GetAddressOf());
		}
		else
		{
			hr = LoadTextureFromFile(device, name, srv.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mTexs.push_back(srv);
	}
	mEditor = std::make_unique<ParticleEditor>();
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void GpuParticleManager::Editor()
{
	std::vector<std::string>nameList;
	for (auto& particle : mParticles)nameList.push_back(particle->GetName());
	bool create, dead;
	int type, index;
	int editType = mEditor->Editor(create, dead, type, index,nameList);

	if (editType == ParticleEditor::EditorType::Operation)
	{
		mParticles[index]->Editor(mDrowTypeNames, mTexs);
		if (dead)DeleteParticle(index);
	}
	else
	{
		if (create)CreateParticle(type);
	}
}
/**************************パーティクル生成関数***************************/
void GpuParticleManager::CreateParticle(int type)
{
	std::string name;
	{
		name = "particle" + std::to_string(mParticles.size());
	}
	mParticles.push_back(std::make_unique<ParticleBase>(Framework::Instance().GetDevice().Get(), type, name));
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void GpuParticleManager::Update(float elapsd_time)
{
	ID3D11DeviceContext* context = Framework::Instance().GetDeviceContext().Get();
	mCbTimer->data.elapsdTime = elapsd_time;
	mCbTimer->Activate(context, 0, false, false, false, true);
	for (auto& particle : mParticles)
	{
		particle->Update(context, elapsd_time);
	}
	mCbTimer->DeActivate(context);
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void GpuParticleManager::Render(ID3D11DeviceContext* context)
{
	for (auto& particle : mParticles)
	{
		//GPU側にデータを送る
		auto& shader = mShaders[particle->GetDrowType()];
		shader->Activate(context);
		PSSetTexture(context, particle->GetDrowType(), particle->GetTexNo());
		particle->Draw(context);
		//GPU側に送ったデータを元に戻す
		shader->Deactivate(context);
		ID3D11ShaderResourceView* srv = nullptr;
		context->PSSetShaderResources(0, 1, &srv);
	}
	//mTitle->Render(context);
}
/************************テクスチャをGPUに送る関数*****************************/

void GpuParticleManager::PSSetTexture(ID3D11DeviceContext* context, int drawType, int texNo)
{
	if (drawType == DrowType::Tex)
	{
		context->PSSetShaderResources(0, 1, mTexs[texNo].GetAddressOf());
	}
}



#endif

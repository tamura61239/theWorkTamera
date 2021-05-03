#include "gpu_curl_noise_particle.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif
#if (PARTICLE_SYSTEM_TYPE==0)

GpuCurlNoiseParticle::GpuCurlNoiseParticle(ID3D11Device* device, const int maxParticle) :GpuParticleTest(device)
{
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/curl_noise_particle_compute.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFE",0,DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	hr = create_vs_from_cso(device, "Data/shader/gpu_curl_noise_particle_render_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_particle_test_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	std::vector<VertexCurlNoise>vertices;
	for (int i = 0;i < maxParticle;i++)
	{
		vertices.push_back(VertexCurlNoise());
		vertices.back().life = static_cast<float>(rand() % 2000) / 1000;
		vertices.back().color = VECTOR4F(0, 0, 0, 0);
		vertices.back().velocity = VECTOR3F(0, 0, 0);
		vertices.back().position = VECTOR4F(0, 0, 0, 0);
	}
	//頂点バッファの作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(VertexCurlNoise) * vertices.size();
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &vertices[0];
		hr = device->CreateBuffer(&desc, &data, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//UAV作成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(VertexCurlNoise) * maxParticle / 4;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//定数バッファ作成
	{
		CD3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(CurlNoise);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		hr = device->CreateBuffer(&desc, nullptr, mCoputeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(&hr), hr_trace(hr));
		desc.ByteWidth = sizeof(NoiseParameter);
		hr = device->CreateBuffer(&desc, nullptr, mNoiseBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(&hr), hr_trace(hr));
	}
	mMaxParticle = maxParticle;
	mVertexSize = sizeof(VertexCurlNoise);
	mCurlNoise.startPosition = VECTOR3F(0, 0, 0);
	mCurlNoise.startColor = VECTOR4F(1, 1, 1, 1);
	mCurlNoise.endColor = VECTOR4F(0, 0, 1, 1);
	mNoiseParameter.maxParticle = mMaxParticle;
	mNoiseParameter.noizeNormalizeFlag = 0;
	mNoiseParameter.noiseSize = 49;
	mNoiseParameter.speed = 1 / 60;
	mNoiseParameter.vec0 = VECTOR3F(0, -100, 0);
	mNoiseParameter.vec1 = VECTOR3F(0, -100, 0);
	mParameter.defSpeedFlag = true;
	mParameter.randSpeed = 1000;
	mParameter.defSpeed = 10;
}

void GpuCurlNoiseParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	if (!mCSShader)return;
#ifdef USE_IMGUI
	ImGui::Begin("curl noise");
	float* position[3] = { &mCurlNoise.startPosition.x,&mCurlNoise.startPosition.y ,&mCurlNoise.startPosition.z };
	ImGui::SliderFloat3("startPosition", *position, -1500, 1500);
	//float* vec0[3] = { &mNoiseParameter.vec0.x,&mNoiseParameter.vec0.y ,&mNoiseParameter.vec0.z };
	//ImGui::SliderFloat3("Vec0", *vec0, -1500, 1500);
	//float* vec1[3] = { &mNoiseParameter.vec1.x,&mNoiseParameter.vec1.y ,&mNoiseParameter.vec1.z };
	//ImGui::SliderFloat3("Vec1", *vec1,-1500,1500);
	float* startColor[4] = { &mCurlNoise.startColor.x,&mCurlNoise.startColor.y ,&mCurlNoise.startColor.z ,&mCurlNoise.startColor.w };
	float* endColor[4] = { &mCurlNoise.endColor.x,&mCurlNoise.endColor.y ,&mCurlNoise.endColor.z ,&mCurlNoise.endColor.w };
	ImGui::ColorEdit4("startColor", *startColor);
	ImGui::ColorEdit4("endColor", *endColor);
	//noiseSize
	static bool normalizeFlag = false;
	ImGui::Checkbox("noiseNormalizeFlag", &normalizeFlag);
	mNoiseParameter.noizeNormalizeFlag = static_cast<float>(normalizeFlag);
	if (!normalizeFlag)
	{
		ImGui::SliderFloat("noiseSize", &mNoiseParameter.noiseSize, 0, 100);
	}
	//speed
	ImGui::Checkbox("defSpeedFlag", &mParameter.defSpeedFlag);
	if (!mParameter.defSpeedFlag)
	{
		ImGui::SliderFloat("speed", &mParameter.defSpeed, -20, 20);
		ImGui::SliderFloat("randSpeed", &mParameter.randSpeed, 1000, 5000);
		std::random_device rnd;     // 非決定的な乱数生成器を生成
		std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
		std::uniform_int_distribution<> randSpeed(1000, static_cast<u_int>(mParameter.randSpeed));
		mNoiseParameter.speed = mParameter.defSpeed / (static_cast<float>(randSpeed(mt)));
	}
	else
	{
		mNoiseParameter.speed = elapsd_time;
	}

	ImGui::DragFloat("maxRand", &mNoiseParameter.maxParticle, 1, 0, mMaxParticle);
	ImGui::End();
#endif
	mCurlNoise.timer = elapsd_time;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetConstantBuffers(0, 1, mCoputeBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mNoiseBuffer.GetAddressOf());
	context->UpdateSubresource(mCoputeBuffer.Get(), 0, 0, &mCurlNoise, 0, 0);
	context->UpdateSubresource(mNoiseBuffer.Get(), 0, 0, &mNoiseParameter, 0, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);

	uavs[0] = nullptr;
	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

}
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
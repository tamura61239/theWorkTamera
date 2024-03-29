#include "gpu_sphere_particle.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"
#if (PARTICLE_SYSTEM_TYPE==0)

GpuSphereParticle::GpuSphereParticle(ID3D11Device* device, const int maxParticle, const float size):GpuParticleTest(device)
{
	std::vector<Vertex>verticis;
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand100(0, 10000);
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/sphere_particle_compute.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = create_vs_from_cso(device, "Data/shader/gpu_particle_test_render_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_particle_test_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	for (int i = 0;i < maxParticle;i++)
	{
		float angle = DirectX::XMConvertToRadians(rand() % 360);
		float z = static_cast<float>(rand() - 16383.5f) / 16383.5f;
		float x = sqrt(1 - z * z) * cosf(angle);
		float y = sqrt(1 - z * z) * sinf(angle);
		float length = static_cast<float>(rand100(mt)) * 0.001f;
		Vertex vertex;
		vertex.position = VECTOR4F(x, y, z , 1.0f)* length;
		vertex.position.w = 1.0f;
		vertex.color = VECTOR4F(1, 1, 1, 1);
		//DirectX::XMStoreFloat3(&vertex.velocity, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&VECTOR3F(vertex.position.x, vertex.position.y, vertex.position.z))));
		vertex.velocity = VECTOR3F(x, y, z) * 0.1f;
		verticis.push_back(vertex);
	}
	mMaxParticle = verticis.size();
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Vertex) * verticis.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		D3D11_SUBRESOURCE_DATA subeData = {};
		subeData.pSysMem = &verticis[0];
		hr = device->CreateBuffer(&desc, &subeData, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	{
		//Viewを作成する
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = sizeof(Vertex) * verticis.size() / 4;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &uavDesc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif

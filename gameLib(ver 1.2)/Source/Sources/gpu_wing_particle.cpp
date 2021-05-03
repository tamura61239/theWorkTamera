#include "gpu_wing_particle.h"
#include<random>
#include<vector>
#include "misc.h"
#include"shader.h"
#include"texture.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif
#if (PARTICLE_SYSTEM_TYPE==0)

GpuWingParticle::GpuWingParticle(ID3D11Device* device, const int maxParticle, const VECTOR3F& centerPosition) :GpuParticleTest(device)
{
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand100(0, 10000);
	HRESULT hr = S_OK;
	mVertexSize = sizeof(VertexWing);
	hr = create_cs_from_cso(device, "Data/shader/wing_particle_compute.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COUNT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = create_vs_from_cso(device, "Data/shader/gpu_wing_particle_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_particle_test_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	std::vector<VertexWing>vertices;
	std::vector<VertexStartParrametr>resetVertices;
	mConstanceWing.centerPosition = centerPosition;
	for (int i = 0;i < maxParticle;i++)
	{
		VertexWing vertex;
		vertex.position = VECTOR4F(0, 0, 0, 0);
		vertex.color = VECTOR4F(0, 0, 0, 0);
		vertex.velocity = VECTOR3F(0, 0, 0);
		vertex.wingCount = 0;
		vertices.push_back(vertex);
		//float angle = DirectX::XMConvertToRadians(rand() % 360);
		//float z = static_cast<float>(rand() - 16383.5f) / 16383.5f;
		//float x = sqrt(1 - z * z) * cosf(angle);
		//float y = sqrt(1 - z * z) * sinf(angle);
		//vertex.position.x = mConstanceWing.centerPosition.x + x;
		//vertex.position.y = mConstanceWing.centerPosition.y + y;
		//vertex.position.z = mConstanceWing.centerPosition.z + z;
		//vertex.position.w = 1;
		//vertex.color = VECTOR4F(1, 1, 1, 1);
		//vertex.wingCount = static_cast<float>(rand() % 2);
		VertexStartParrametr startVertex;
		startVertex.length = static_cast<float>(rand100(mt)) * 0.0001f;
		startVertex.wingCount = static_cast<float>(rand() % 4);
		float angle = DirectX::XMConvertToRadians(rand() % 360);
		float z = static_cast<float>(rand() - 16383.5f) / 16383.5f;
		float x = sqrt(1 - z * z) * cosf(angle);
		float y = sqrt(1 - z * z) * sinf(angle);
		startVertex.position = VECTOR4F(x, y, z, 1.0f);
		resetVertices.push_back(startVertex);
	}
	mMaxParticle = maxParticle;
	//バッファ&view生成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Vertex) * mMaxParticle;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		D3D11_SUBRESOURCE_DATA subeData = {};
		subeData.pSysMem = &vertices[0];
		hr = device->CreateBuffer(&desc, &subeData, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = sizeof(VertexStartParrametr) * mMaxParticle;
		subeData.pSysMem = &resetVertices[0];
		hr = device->CreateBuffer(&desc, &subeData, mResetBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	{
		//Viewを作成する
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = sizeof(Vertex) * mMaxParticle / 4;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &uavDesc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		uavDesc.Buffer.NumElements = sizeof(VertexStartParrametr) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mResetBuffer.Get(), &uavDesc, mResetUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(ConstanceWing);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = device->CreateBuffer(&desc, nullptr, mCoputeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	VECTOR4F center = VECTOR4F(mConstanceWing.centerPosition.x, mConstanceWing.centerPosition.y, mConstanceWing.centerPosition.z, 1);
	mConstanceWing.indexSize = 100;
	mConstanceWing.startIndex = 0;
	mConstanceWing.wingEndPoint[0].color = VECTOR4F(1, 0, 0, 1);
	mConstanceWing.wingEndPoint[1].color = VECTOR4F(0, 1, 0, 1);
	mConstanceWing.wingEndPoint[2].color = VECTOR4F(0, 0, 1, 1);
	mConstanceWing.wingEndPoint[3].color = VECTOR4F(1, 1, 1, 1);
	mConstanceWing.wingEndPoint[0].position = VECTOR4F(10, 3, 0, 1);
	mConstanceWing.wingEndPoint[1].position = VECTOR4F(-10, 3, 0, 1);
	mConstanceWing.wingEndPoint[2].position = VECTOR4F(20, 0, 0, 1);
	mConstanceWing.wingEndPoint[3].position = VECTOR4F(-20, 0, 0, 1);
	mConstanceWing.wingEndPoint[0].size = VECTOR4F(1, 0.3, 1, 1);
	mConstanceWing.wingEndPoint[1].size = VECTOR4F(1, 0.3, 1, 1);
	mConstanceWing.wingEndPoint[2].size = VECTOR4F(0.3f, 0.1, 0.3f, 1);
	mConstanceWing.wingEndPoint[3].size = VECTOR4F(0.3f, 0.1, 0.3f, 1);

}

void GpuWingParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() ,mResetUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetConstantBuffers(0, 1, mCoputeBuffer.GetAddressOf());
	context->UpdateSubresource(mCoputeBuffer.Get(), 0, 0, &mConstanceWing, 0, 0);
	static float angle = 0;
	angle += DirectX::XMConvertToRadians(1);
	float si = sinf(angle);
	mConstanceWing.wingEndPoint[0].position.y += si * 0.03f;
	mConstanceWing.wingEndPoint[1].position.y += si * 0.03f;
	mConstanceWing.wingEndPoint[2].position.y += si * 0.08f;
	mConstanceWing.wingEndPoint[3].position.y += si * 0.08f;
	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);
	if (mConstanceWing.startIndex <= mMaxParticle)
	{
		mConstanceWing.startIndex += mConstanceWing.indexSize;
	}
	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
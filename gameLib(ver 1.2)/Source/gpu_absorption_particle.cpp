#include "gpu_absorption_particle.h"
#include<random>
#include<vector>
#include "misc.h"
#include"shader.h"


GpuAbsorptionParticle::GpuAbsorptionParticle(ID3D11Device* device, int maxParticle, int size, VECTOR3F center) :GpuParticleTest(device)
{
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand100(0, 10000);
	std::uniform_int_distribution<> rand2(0, 20000);
	std::uniform_int_distribution<> randAngle(0, 6280000);

	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/absorpartion_particle_compute.cso", mCSShader.GetAddressOf());
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

	std::vector<Vertex>vertices;
	std::vector<Vertex>resetVertices;
	for (int i = 0;i < maxParticle;i++)
	{
		Vertex vertex;
		vertex.position = VECTOR4F(0, 0, 0, 1);
		vertex.velocity = VECTOR3F(0, 0, 0);
		vertex.color = VECTOR4F(0, 0, 0, 0);
		float length = static_cast<float>(rand100(mt)) * 0.0001f;
		length *= size;
		float angle = static_cast<float>(randAngle(mt)) * 0.000001f;
		vertex.position.x = center.x + sinf(angle) * length;
		vertex.position.z = center.z + cosf(angle) * length;
		vertices.push_back(vertex);
		vertex.position.x = center.x + sinf(angle) * size;
		vertex.position.z = center.z + cosf(angle) * size;
		resetVertices.push_back(vertex);
	}
	mMaxParticle = vertices.size() - 1;
	//バッファ&view生成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Vertex) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		D3D11_SUBRESOURCE_DATA subeData = {};
		subeData.pSysMem = &vertices[0];
		hr = device->CreateBuffer(&desc, &subeData, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
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
		uavDesc.Buffer.NumElements = sizeof(Vertex) * vertices.size() / 4;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &uavDesc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateUnorderedAccessView(mResetBuffer.Get(), &uavDesc, mResetUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	mComputeConstance.centerPosition = VECTOR3F(0, 0, 0);
	mComputeConstance.size = static_cast<float>(size);
	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(ComputeConstance);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mConputeConstanceBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

void GpuAbsorptionParticle::Update(ID3D11DeviceContext* context)
{
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() ,mResetUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetConstantBuffers(0, 1, mConputeConstanceBuffer.GetAddressOf());
	context->UpdateSubresource(mConputeConstanceBuffer.Get(), 0, 0, &mComputeConstance, 0, 0);

	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);

	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}

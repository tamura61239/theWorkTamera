#include "gpu_swirl_particle.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"

GpuSwirlParticle::GpuSwirlParticle(ID3D11Device* device, int maxParticle) :GpuParticleTest(device)
{
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand100(0, 10000);
	std::uniform_int_distribution<> rand2(0, 20000);

	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/swirl_particle_compute.cso", mCSShader.GetAddressOf());
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
	for (int i = 0;i < maxParticle;i++)
	{
		Vertex vertex;
		vertex.position = VECTOR4F(0, 0, 0, 1);
		float length = static_cast<float>(rand100(mt)) * 0.0001f;
		int type = rand() % 4;
		if (type <= 1)type = 0;
		else type = 1;
		int a = length / 0.4f * 60;
		float angle = DirectX::XMConvertToRadians(static_cast<float>(a) + 180 * static_cast<float>(type));

		vertex.position.x += sinf(angle) * 10.0f * length + sinf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;
		vertex.position.z += cosf(angle) * 10.0f * length + cosf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;

		vertex.color = VECTOR4F(1, 1, 1, 1);
		if (length > 0.4f)
		{
			vertex.color.w = 1.f - (length - 0.4f) / 0.6f;
		}
		vertex.velocity = VECTOR3F(0, 0, 0);
		vertices.push_back(vertex);
	}
	mMaxParticle = vertices.size();
	std::vector<Vertex>resetVritices;
	for (auto&vec:vertices)
	{
		Vertex vertex;
		vertex.position = VECTOR4F(0, 0, 0, 1);

		float length = 10000 * 0.0001f;
		int type = rand() % 4;
		if (type <= 1)type = 0;
		else type = 1;
		int a = length / 0.4f * 60;
		float angle = DirectX::XMConvertToRadians(static_cast<float>(a) + 180 * static_cast<float>(type));

		vertex.position.x += sinf(angle) * 10.0f * length + sinf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;
		vertex.position.z += cosf(angle) * 10.0f * length + cosf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;

		vertex.color = VECTOR4F(1, 1, 1, 1);
		//if (length > 0.4f)
		//{
		//	vertex.color.w = 1.f - (length - 0.4f) / 0.6f;
		//}
		vertex.velocity = VECTOR3F(0, 0, 0);
		resetVritices.push_back(vertex);

	}
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
		subeData.pSysMem = &resetVritices[0];
		hr = device->CreateBuffer(&desc, &subeData, mResetVertex.GetAddressOf());
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

		hr = device->CreateUnorderedAccessView(mResetVertex.Get(), &uavDesc, mResetUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	mComputeConstance.centerPosition = VECTOR3F(0, 0, 0);
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

void GpuSwirlParticle::Update(ID3D11DeviceContext* context)
{
	if (!mCSShader)return;
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> rand100(0, 10000);
	std::uniform_int_distribution<> rand2(0, 20000);
	std::random_device rnd2;     // 非決定的な乱数生成器を生成
	//std::mt19937 mt2(rnd2());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	//std::uniform_int_distribution<> rand3(0, 2000);
	static float angle = 0;
	angle += 0.01745f;
	//mComputeConstance.;
	//int type = rand3(mt2);
	//if (type <= 1000)type = 1;
	//else type = 0;
	//mComputeConstance.type = static_cast<float>(type);
	//mComputeConstance.x += sinf(angle) * 10.0f * length + sinf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;
	//mComputeConstance.z += cosf(angle) * 10.0f * length + cosf(angle + DirectX::XMConvertToRadians(90.0f)) * (rand2(mt) - 10000) * 0.0001f * length * 3.f;
	if (angle >= 3.14f)angle -= 3.14f;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get(),mResetUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetConstantBuffers(1, 1, mConputeConstanceBuffer.GetAddressOf());
	context->UpdateSubresource(mConputeConstanceBuffer.Get(), 0, 0, &mComputeConstance, 0, 0);
	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);

	uavs[0] = nullptr;
	uavs[1] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}

//void GpuSwirlParticle::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
//{
//}

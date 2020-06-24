#include "gpu_2d_texture_particle.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"
#include"screen_size.h"

Gpu2DTextureParticle::Gpu2DTextureParticle(ID3D11Device* device, VECTOR2F size):GpuParticleTest(device)
{
	HRESULT hr = S_OK;

	hr = create_cs_from_cso(device, "Data/shader/2d_texture_particle_compute.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = create_vs_from_cso(device, "Data/shader/gpu_particle_2d_test_render_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_particle_2d_test_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mUV = size;
	VECTOR2F textureStart = VECTOR2F(SCREEN_WIDTH - size.x, SCREEN_HEIGHT - size.y) * 0.5f;
	VECTOR2F view = VECTOR2F(1.f / static_cast<float>(SCREEN_WIDTH), 1.f / static_cast<float>(SCREEN_HEIGHT));
	std::vector<Vertex>vertices;
	for (int x = 0;x < size.x;x++)
	{
		for (int y = 0;y < size.y;y++)
		{
			Vertex vertex;
			vertex.position = VECTOR4F((textureStart.x + x) * view.x, (textureStart.y + y) * view.y, 0, 1);
			vertex.position.x = vertex.position.x * 2.f - 1.f;
			vertex.position.y = vertex.position.y * 2.f - 1.f;
			vertex.color = VECTOR4F(1, 1, 1, 1);
			//vertex.velocity = VECTOR3F(vertex.position.x, vertex.position.y, 0)*0.01f;
			vertex.velocity = VECTOR3F(0, 0, 0);
			vertices.push_back(vertex);
		}
	}
	mMaxParticle = vertices.size();
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

	}
	{
		//View‚ðì¬‚·‚é
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = sizeof(Vertex) * vertices.size() / 4;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &uavDesc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

void Gpu2DTextureParticle::Update(ID3D11DeviceContext* context)
{
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);

	//ŽÀs
	context->Dispatch(mUV.x/10, mUV.y / 10, 1);

	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}



#include "gpu_curl_noise_particle.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"

GpuCurlNoiseParticle::GpuCurlNoiseParticle(ID3D11Device* device, const int maxParticle):GpuParticleTest(device)
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
		vertices.back().life = rand()%6;
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
	}
	mMaxParticle = maxParticle;
	mVertexSize = sizeof(VertexCurlNoise);
	mCurlNoise.startPosition = VECTOR3F(0, 0, 0);
	mCurlNoise.startColor = VECTOR4F(1, 1, 1, 1);
	mCurlNoise.endColor = VECTOR4F(1, 1, 1, 1);
}

void GpuCurlNoiseParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	if (!mCSShader)return;
	mCurlNoise.timer = elapsd_time;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetConstantBuffers(0, 1, mCoputeBuffer.GetAddressOf());
	context->UpdateSubresource(mCoputeBuffer.Get(), 0, 0, &mCurlNoise, 0, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);

	uavs[0] = nullptr;
	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

}

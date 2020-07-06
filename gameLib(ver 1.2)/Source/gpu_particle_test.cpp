#include "gpu_particle_test.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"


GpuParticleTest::GpuParticleTest(ID3D11Device* device) :mMaxParticle(0),mVertexSize(sizeof(Vertex))
{
	mObj = std::make_unique<Obj3D>();
	mObj->CalculateTransform();
	HRESULT hr = S_OK;

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// 深度ステンシルステートの設定
	{
		//D3D11_DEPTH_STENCIL_DESC desc;
		//::memset(&desc, 0, sizeof(desc));
		//desc.DepthEnable = true;
		//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		//hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());

		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
		depth_stencil_desc.DepthEnable = false;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = false;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CbScene);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbSceneBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		bufferDesc.ByteWidth = sizeof(CbObj);

		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbObjBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

	// create sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	memcpy(samplerDesc.BorderColor, &VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f), sizeof(VECTOR4F));
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	hr = device->CreateSamplerState(&samplerDesc, mSamplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void GpuParticleTest::Update(ID3D11DeviceContext* context,float elapsd_time)
{
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);

	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);

	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}

void GpuParticleTest::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	ID3D11Buffer* constant_buffers[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizerState.Get());
	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
	CbObj cbObj;
	cbObj.world = mObj->GetWorld();
	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cbObj, 0, 0);
	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());

	UINT stride = mVertexSize;
	UINT offset = 0;
	ID3D11Buffer* vbs[]
	{
		mVertexBuffer.Get()
	};
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->PSSetShaderResources(0, 0, nullptr);
	context->Draw(mMaxParticle, 0);
	vbs[0] = nullptr;
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);

}


#include "gpu_particle_test.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"

#if (PARTICLE_SYSTEM_TYPE==0)

GpuParticleTest::GpuParticleTest(ID3D11Device* device) :mMaxParticle(0), mVertexSize(sizeof(Vertex))
{
	mObj = std::make_unique<Obj3D>();
	mObj->CalculateTransform();
	HRESULT hr = S_OK;

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
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
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
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

void GpuParticleTest::Update(ID3D11DeviceContext* context, float elapsd_time)
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
#elif (PARTICLE_SYSTEM_TYPE==1)
GpuParticle::GpuParticle(ID3D11Device* device, std::string name, std::string filePath) :mIndexCount(0), mParticleCount(0), mMaxParticle(500000), mCreateCount(0), mCountCrease(1)
{
	//パーティクルバッファの生成
	std::vector<ParticleData>particleData;
	particleData.resize(mMaxParticle);
	mParticle = std::make_unique<ParticleBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particleData.data(), true);
	//描画用データの生成
	std::vector<RenderParticle>renderData;
	renderData.resize(mMaxParticle);
	mRender = std::make_unique<ParticleBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderData.data(), true);
	//Indexデータの生成
	std::vector<UINT>indices;
	indices.resize(mMaxParticle);
	memset(indices.data(), 0, sizeof(UINT) * mMaxParticle);
	for (int i = 0; i < 2; i++)mIndex[i] = std::make_unique<ParticleBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices.data(), true);
	for (int i = 0; i < mMaxParticle; i++)
	{
		indices[i] = i;
	}
	mDeathIndex = std::make_unique<ParticleBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices.data(), true);
	ParticleCount count;
	memset(&count, 0, sizeof(ParticleCount));
	count.deActiveParticleCount = mMaxParticle;
	mCount = std::make_unique<ParticleBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, &count, true);
	//定数バッファ生成
	mCreateData = std::make_unique<ConstantBuffer<CreateData>>(device);
	HRESULT hr = S_OK;
	//シェーダーの生成
	hr = CreateCSFromCso(device, "Data\shader\point_create_particle.cso", mCresteShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data\shader\particle_last_move.cso", mLastMoveShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data\shader\particle_count_reset.cso", mCountResetShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = name;
	mFilePath = filePath;
}

void GpuParticle::Editor()
{
}

void GpuParticle::Update(ID3D11DeviceContext* context)
{
	mParticle->Activate(context, 0, true);
	for (int i = 0; i < 2; i++)mIndex[mIndexCount + i]->Activate(context, 1 + i, true);
	mDeathIndex->Activate(context, 3, true);
	mRender->Activate(context, 4, true);
	mCount->Activate(context, 5, true);
	//生成
	mCreateData->Activate(context, 1, false, false, false, true);
	context->CSSetShader(mCresteShader.Get(), nullptr, 0);
	context->Dispatch(mCreateCount, 1, 1);
	mIndexCount += mCountCrease;
	mCountCrease *= -1;
	//更新
	for (auto& move : mMoves)
	{
		move->Move(context, mMaxParticle / 100);
	}

	//描画用のバッファにデータを移す
	context->CSSetShader(mLastMoveShader.Get(), nullptr, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);


	//カウントを更新
	context->CSSetShader(mCountResetShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//カウントのバッファから生きてる分のカウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mParticleCount = particleCount->aliveParticleCount;
	context->Unmap(mCount->GetBuffer(), NULL);

	//後処理
	context->CSSetShader(nullptr, nullptr, 0);
	mCreateData->DeActivate(context);
	mParticle->DeActivate(context);
	for (int i = 0; i < 2; i++)mIndex[mIndexCount + i]->Activate(context, 1 + i, true);
	mDeathIndex->DeActivate(context);
	mRender->DeActivate(context);
	mCount->DeActivate(context);

}

void GpuParticle::Render(ID3D11DeviceContext* context)
{
	ID3D11Buffer* buffer = mRender->GetBuffer();
	ID3D11Buffer* index = mIndex[mIndexCount]->GetBuffer();
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mParticleCount, 0, 0);

	buffer = nullptr;
	index = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}

#endif


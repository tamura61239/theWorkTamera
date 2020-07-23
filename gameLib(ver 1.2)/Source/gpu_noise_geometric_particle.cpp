#include "gpu_noise_geometric_particle.h"
#include<vector>
#include "misc.h"
#include"shader.h"
#include"texture.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif

GpuNoiseGeometricParticle::GpuNoiseGeometricParticle(ID3D11Device* device, const int maxParticle) :GpuParticleTest(device)
{
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/noise_geometric_particle.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ANGLE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "LIFE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MAXLIFE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = create_vs_from_cso(device, "Data/shader/gpu_noise_geometric_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_noise_geometric_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_gs_from_cso(device, "Data/shader/gpu_noise_geometric_gs.cso", mGSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	std::vector<VertexGeometric>vertices;
	
	vertices.resize(maxParticle);
	for (auto& v : vertices)
	{
		v.color = VECTOR4F(0, 0, 0, 0);
		v.angle = VECTOR3F(0, 0, 0);
		v.position = VECTOR4F(0, 0, 0, 0);
		v.maxLife = 0;
		v.life = 0;
		v.velocity = VECTOR3F(0, 0, 0);
	}
	mMaxParticle = maxParticle;
	mVertexSize = sizeof(VertexGeometric);
	//バッファ&view生成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(VertexGeometric) * mMaxParticle;
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
		//Viewを作成する
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = sizeof(VertexGeometric) * mMaxParticle / 4;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		hr = device->CreateUnorderedAccessView(mVertexBuffer.Get(), &uavDesc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(CbMoveDataBuffer);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = device->CreateBuffer(&desc, nullptr, mCoputeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbStartBuffer);
		hr = device->CreateBuffer(&desc, nullptr, mCbStartBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	cbStartBuffer.indexSize = 30;
	cbStartBuffer.startIndex = 0;
	cbMoveDataBuffer.angleSpeed = VECTOR3F(0, 0, 0);
	cbStartBuffer.setLife = 10;
	cbStartBuffer.timer = 1;
	cbMoveDataBuffer.standardVelocity = VECTOR3F(0, 0.1, 0);
	cbMoveDataBuffer.startPosition = VECTOR3F(0, 0, 0);
	cbStartBuffer.startColor = VECTOR4F(1, 1, 1, 1);
}

void GpuNoiseGeometricParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{
#ifdef USE_IMGUI
	ImGui::Begin("NoiseGeometric");
	float* startPosition[3] = { &cbMoveDataBuffer.startPosition.x,&cbMoveDataBuffer.startPosition.y ,&cbMoveDataBuffer.startPosition.z };
	ImGui::SliderFloat3("startPosition", *startPosition, -100, 100);
	float* standardVelocity[3] = { &cbMoveDataBuffer.standardVelocity.x,&cbMoveDataBuffer.standardVelocity.y ,&cbMoveDataBuffer.standardVelocity.z };
	ImGui::InputFloat3("standardVelocity", *standardVelocity);
	float* angleSpeed[3] = { &cbMoveDataBuffer.angleSpeed.x,&cbMoveDataBuffer.angleSpeed.y ,&cbMoveDataBuffer.angleSpeed.z };
	ImGui::SliderFloat3("angleSpeed", *angleSpeed, -3.14, 3.14);
	float* startColor[4] = { &cbStartBuffer.startColor.x,&cbStartBuffer.startColor.y ,&cbStartBuffer.startColor.z,&cbStartBuffer.startColor.w };
	ImGui::SliderFloat4("startColor", *startColor, 0, 1);

	ImGui::End();
#endif
	cbStartBuffer.timer = elapsd_time;
	cbStartBuffer.startIndex += cbStartBuffer.indexSize*elapsd_time;
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get()};
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetConstantBuffers(0, 1, mCoputeBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStartBuffer.GetAddressOf());
	context->UpdateSubresource(mCoputeBuffer.Get(), 0, 0, &cbMoveDataBuffer, 0, 0);
	context->UpdateSubresource(mCbStartBuffer.Get(), 0, 0, &cbStartBuffer, 0, 0);

	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);
	if (mMaxParticle < cbStartBuffer.startIndex )
	{
		cbStartBuffer.startIndex -= mMaxParticle;
	}

	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}

void GpuNoiseGeometricParticle::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	ID3D11Buffer* constant_buffers[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->GSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
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
	context->GSSetShader(mGSShader.Get(), nullptr, 0);
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
	context->Draw(mMaxParticle, 0);
	vbs[0] = nullptr;
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
	context->GSSetShader(nullptr, nullptr, 0);

}

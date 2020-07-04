#include "gpu_cloud_particle.h"
#include<random>
#include<vector>
#include "misc.h"
#include"shader.h"
#include"texture.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif


GpuCloudParticle::GpuCloudParticle(ID3D11Device* device, int maxParticle, const VECTOR2F& maxLength, const VECTOR3F& cloudDirection, const VECTOR3F& startPosition):GpuParticleTest(device)
{
	static const float maxRandNumber = 10000;
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> randPosition(0, maxRandNumber*2);
	std::uniform_int_distribution<> randSize(1, 100);
	std::uniform_int_distribution<> randMass(1, 100);
	HRESULT hr = S_OK;
	hr = create_cs_from_cso(device, "Data/shader/cloud_particle_compute.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
#if 0
	hr = create_vs_from_cso(device, "Data/shader/gpu_clound_particle_render_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_cloud_particle_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_gs_from_cso(device, "Data/shader/gpu_cloud_particle_render_gs.cso", mGSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#else
	hr = create_vs_from_cso(device, "Data/shader/gpu_particle_test_render_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_ps_from_cso(device, "Data/shader/gpu_particle_test_render_ps.cso", mPSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
	CD3D11_TEXTURE2D_DESC tex2dDesc;
	hr = load_texture_from_file(device, L"Data/image/particle-smoke.png", mSRV.GetAddressOf(), &tex2dDesc);
	VECTOR2F randRatio = maxLength / maxRandNumber;
	std::vector<Vertex>vertices;
	std::vector<VertexCloud>resetVertices;
	for (int i = 0;i < maxParticle;i++)
	{
		Vertex vertex;
		vertex.position = VECTOR4F(0, 0, 0, 0);
		vertex.color = VECTOR4F(0, 0, 0, 0);
		vertex.velocity = VECTOR3F(0, 0, 0);
		vertices.push_back(vertex);
		VertexCloud vertexCloud;
		vertexCloud.position.x = startPosition.x + static_cast<float>(randPosition(mt)-maxRandNumber)* randRatio.x;
		vertexCloud.position.z = startPosition.z + static_cast<float>(randPosition(mt)-maxRandNumber)* randRatio.y;
		vertexCloud.position.w = 1.0f;
		vertexCloud.color = VECTOR4F(1, 1, 1, 0);
		vertexCloud.cloudSize = static_cast<float>(randSize(mt));
		vertexCloud.cloudMass = static_cast<float>(randMass(mt))* vertexCloud.cloudSize*0.8f;
		vertexCloud.position.y = startPosition.y + vertexCloud.cloudSize*0.1f;
		resetVertices.push_back(vertexCloud);
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
		desc.ByteWidth = sizeof(VertexCloud) * mMaxParticle;
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
		uavDesc.Buffer.NumElements = sizeof(VertexCloud) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mResetBuffer.Get(), &uavDesc, mResetUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(ComputeComstance);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hr = device->CreateBuffer(&desc, nullptr, mCoputeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(SwirlComstance);
		hr = device->CreateBuffer(&desc, nullptr, mSwirlBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	for (int i = 0;i < 10;i++)
	{
		mSwirlComstance.swirls[i].centerPosition = VECTOR3F(0, 0, 0);
		mSwirlComstance.swirls[i].length = 0;
		mSwirlComstance.swirls[i].flag = 0;
		mSwirlComstance.swirls[i].dummy = VECTOR3F(0, 0, 0);
	}
	mConputeComstance.length = maxLength*5;
	mConputeComstance.startIndex = 0;
	mConputeComstance.indexSize = 50;
	mConputeComstance.windVelocity = VECTOR3F(0, 0, 1);
}

void GpuCloudParticle::Update(ID3D11DeviceContext* context)
{
#ifdef USE_IMGUI
	ImGui::Begin("cloudParticle");
	static int number = 0;
	const char* swirlName[] = { "swirl 01","swirl 02" ,"swirl 03" ,"swirl 04" ,"swirl 05" ,"swirl 06" ,"swirl 07" ,"swirl 08" ,"swirl 09" ,"swirl 10" };
	ImGui::Combo("swirl", &number, swirlName, static_cast<int>(sizeof(swirlName) / sizeof(*swirlName)));
	float* position[] = { &mSwirlComstance.swirls[number].centerPosition.x,&mSwirlComstance.swirls[number].centerPosition.y ,&mSwirlComstance.swirls[number].centerPosition.z };
	ImGui::SliderFloat3("centerPosition", *position, -1000, 1000);
	ImGui::SliderFloat("length", &mSwirlComstance.swirls[number].length, 0, 200);
	const char* button[] = { "ON","OFF" };
	if (ImGui::Button(button[static_cast<int>(mSwirlComstance.swirls[number].flag)]))
	{
		mSwirlComstance.swirls[number].flag++;
		if (mSwirlComstance.swirls[number].flag >= 2)mSwirlComstance.swirls[number].flag = 0;
	}
	ImGui::End();
#endif
	if (!mCSShader)return;
	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() ,mResetUAV.Get() };
	ID3D11ComputeShader* compute = mCSShader.Get();
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(compute, nullptr, 0);
	context->CSSetConstantBuffers(0, 1, mCoputeBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mSwirlBuffer.GetAddressOf());
	context->UpdateSubresource(mCoputeBuffer.Get(), 0, 0, &mConputeComstance, 0, 0);
	context->UpdateSubresource(mSwirlBuffer.Get(), 0, 0, &mSwirlComstance, 0, 0);

	//実行
	context->Dispatch(mMaxParticle / 100, 1, 1);
	if (mConputeComstance.startIndex <= mMaxParticle)
	{
		mConputeComstance.startIndex += mConputeComstance.indexSize;
	}
	uavs[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	compute = nullptr;
	context->CSSetShader(compute, nullptr, 0);

}

void GpuCloudParticle::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	ID3D11Buffer* constant_buffers[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
//	context->GSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
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
//	context->GSSetShader(mGSShader.Get(), nullptr, 0);
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
	//context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());
	//context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->Draw(mMaxParticle, 0);
	vbs[0] = nullptr;
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
	context->GSSetShader(nullptr, nullptr, 0);
}

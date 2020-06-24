#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"obj3d.h"
#include<memory>

class GpuParticleTest
{
public:
	GpuParticleTest(ID3D11Device* device);
	virtual void Update(ID3D11DeviceContext* context);
	virtual Obj3D* GetObj() { return mObj.get(); }
	virtual void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	struct Vertex
	{
		VECTOR4F position;
		VECTOR3F velocity;
		VECTOR4F color;
	};
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	struct CbObj
	{
		FLOAT4X4 world;
	};
protected:
	std::unique_ptr<Obj3D>mObj;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCoputeBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbSceneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbObjBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;

	float mMaxParticle;
	UINT mVertexSize;
};
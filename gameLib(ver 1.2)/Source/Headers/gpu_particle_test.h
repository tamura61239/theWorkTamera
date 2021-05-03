#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"obj3d.h"
#include<memory>
#include<string>
#include"particle_buffer.h"
#include"constant_buffer.h"
#include"particle_move.h"
#include<vector>
#define PARTICLE_SYSTEM_TYPE 1

#if (PARTICLE_SYSTEM_TYPE==0)
class GpuParticleTest
{
public:
	GpuParticleTest(ID3D11Device* device);
	virtual void Update(ID3D11DeviceContext* context, float elapsd_time);
	//virtual Obj3D* GetObj() { return mObj.get(); }
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
#elif (PARTICLE_SYSTEM_TYPE==1)
class GpuParticle
{
public:
	GpuParticle(ID3D11Device* device, std::string name,std::string fileName = "");
	virtual void Editor();
	virtual void Update(ID3D11DeviceContext* context);
	virtual void Render(ID3D11DeviceContext* context);
	std::string GetName() { return mName; }
	std::string GetFilePath() { return mFilePath; }
protected:
	struct ParticleData
	{
		VECTOR3F position;
		VECTOR3F scale;
		VECTOR3F angle;
		VECTOR3F velocity;
		VECTOR4F color;
		VECTOR3F accel;
		float life;
	};
	struct RenderParticle
	{
		VECTOR3F position;
		VECTOR3F scale;
		VECTOR3F angle;
		VECTOR3F velocity;
		UINT color;
	};
	struct ParticleCount
	{
		UINT aliveParticleCount;
		UINT aliveNewParticleCount;
		UINT deActiveParticleCount;
		UINT dummy;
	};
	//バッファ
	std::unique_ptr<ParticleBuffer>mParticle;
	std::unique_ptr<ParticleBuffer>mRender;
	std::unique_ptr<ParticleBuffer>mCount;
	std::unique_ptr<ParticleBuffer>mIndex[2];
	std::unique_ptr<ParticleBuffer>mDeathIndex;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCresteShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mLastMoveShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCountResetShader;
	//変数
	UINT mParticleCount;
	UINT mMaxParticle;
	UINT mCreateCount;
	UINT mIndexCount;
	int mCountCrease;
	enum class RenderObjType
	{
		point,board,texture,cube,
	};
	std::vector<std::unique_ptr<ParticleMove>>mMoves;
	std::string mName;
	std::string mFilePath;
private:
	struct CreateData
	{
		VECTOR3F position;
		float speed;
		VECTOR4F color;
		float life;
		VECTOR3F scale;
		float accel;
		VECTOR3F dummy;
	};

	//定数バッファ
	std::unique_ptr<ConstantBuffer<CreateData>>mCreateData;
};
#endif
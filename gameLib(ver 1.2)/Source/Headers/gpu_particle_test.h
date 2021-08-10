#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"obj3d.h"
#include<memory>
#include<string>
#include"cs_buffer.h"
#include"constant_buffer.h"
#include"particle_move.h"

#include<vector>
#include <functional>
#include"drow_shader.h"
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
	//コンストラクタ
	GpuParticle(ID3D11Device* device, std::string name,std::string fileName = "");
	//エディタ

	virtual void Editor(std::vector<std::string>shaders,std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>texs);
	//更新
	virtual void Update(ID3D11DeviceContext* context,float elapsdTime);
	//描画
	virtual void Render(ID3D11DeviceContext* context);
	//getter
	std::string GetName() { return mName; }
	std::string GetFilePath() { return mFilePath; }
	const int GetDrowType() { return mDrowType; }
	const int GetTexNo() { return mTexNo; }
protected:
	void DrowEditorFunction(void* id, std::vector<std::string>shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>texs);
	//パーティクルデータ
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
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct ParticleCount
	{
		UINT aliveParticleCount;
		UINT aliveNewParticleCount;
		UINT deActiveParticleCount;
		UINT dummy;
	};
	//バッファ
	std::unique_ptr<CSBuffer>mParticle;
	std::unique_ptr<CSBuffer>mRender;
	std::unique_ptr<CSBuffer>mCount;
	std::unique_ptr<CSBuffer>mIndex[2];
	std::unique_ptr<CSBuffer>mDeathIndex;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCresteShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mLastMoveShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCountResetShader;
	//変数
	UINT mParticleCount;
	UINT mMaxParticle;
	int mCreateCount;
	UINT mIndexCount;
	int mCountCrease;
	int mDrowType;
	int mTexNo;
	//パーティクルの動き
	std::vector<std::unique_ptr<ParticleMove>>mMoves;
	//名前
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
	struct CbTimer
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	//定数バッファ
	std::unique_ptr<ConstantBuffer<CreateData>>mCreateData;
	std::unique_ptr<ConstantBuffer<CbTimer>>mCbTimer;
};
//class TitleParticle
//{
//public:
//	TitleParticle(ID3D11Device* device);
//	void Editor();
//	void Update(float elapsdTime, ID3D11DeviceContext* context);
//	void Render(ID3D11DeviceContext* context);
//	void SetChangeFlag(const bool changeFlag) { mSceneChange = changeFlag; }
//private:
//	//パーティクルバッファ
//	std::unique_ptr<CSBuffer>mParticle;
//	std::unique_ptr<CSBuffer>mParticleCount;
//	//描画用バッファ
//	std::unique_ptr<CSBuffer>mParticleRender;
//	std::unique_ptr<CSBuffer>mParticleIndexs[2];
//	std::unique_ptr<CSBuffer>mParticleDeleteIndex;
//	//シェーダー
//	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
//	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
//	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mSceneChangeCSShader;
//	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mRenderSetCSShader;
//	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCountShader;
//	std::unique_ptr<DrowShader>mShader;
//	//srv
//	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mParticleSRV;
//	//パーティクルのデータ
//	struct Particle
//	{
//		VECTOR3F position;
//		VECTOR3F scale;
//		VECTOR3F velocity;
//		VECTOR3F angle;
//		VECTOR4F color;
//		float speed;
//		float life;
//		float moveAngle;
//		float moveAngleMovement;
//		float moveAngleLength;
//		float maxLife;
//		VECTOR3F defVelocity;
//		VECTOR3F startPosition;
//		VECTOR3F defPosition;
//
//	};
//	struct RenderParticle
//	{
//		VECTOR4F position;
//		VECTOR3F angle;
//		VECTOR4F color;
//		VECTOR3F velocity;
//		VECTOR3F scale;
//	};
//	struct ParticleCount
//	{
//		UINT aliveParticleCount;
//		UINT aliveNewParticleCount;
//		UINT deActiveParticleCount;
//		UINT dummy;
//	};
//
//	//定数バッファ
//	struct CbStart
//	{
//		float startIndex;
//		VECTOR3F startPosition;
//		float leng;
//		VECTOR3F sphereRatio;
//		VECTOR4F color;
//		float life;
//		float moveLen;
//		float randSpeed;
//		float defSpeed;
//
//	};
//	struct CbStart2
//	{
//		float randMoveLength;
//		float defMoveLength;
//		float randMoveAngle;
//		float randScale;
//		VECTOR3F defVelocity;
//		float dummy;
//
//	};
//	struct Cb
//	{
//		float elapsdTime;
//		VECTOR3F angleMovement;
//	};
//	std::unique_ptr<ConstantBuffer<CbStart>>mCbStart;
//	std::unique_ptr<ConstantBuffer<CbStart2>>mCbStart2;
//	std::unique_ptr<ConstantBuffer<Cb>>mCb;
//	//エディタデータ
//	struct EditorData
//	{
//		VECTOR3F startPosition;
//		float leng;
//		VECTOR3F sphereRatio;
//		VECTOR4F color;
//		float life;
//		float moveLen;
//		float randSpeed;
//		float defSpeed;
//		float randMoveLength;
//		float defMoveLength;
//		float randMoveAngle;
//		float randScale;
//		VECTOR3F defVelocity;
//		VECTOR3F angleMovement;
//		UINT textureType;
//	};
//	EditorData mEditorData;
//	//数
//	float mStartIdex;
//	//生成数
//	float particleSize;
//	//最大数
//	int mMaxParticle;
//	float mNewIndex;
//	//シーンを切り替えるかどうか
//	bool mSceneChange;
//	//描画用データ
//	UINT mRenderCount;
//	int mIndexNum;
//
//};
#endif
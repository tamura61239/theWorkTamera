#pragma once
#include"cs_buffer.h"
#include"constant_buffer.h"
#include<string>
#include<memory>
#include"particle_move.h"
#include"drow_shader.h"
#include"particle_create_point.h"

class ParticleBase
{
public:
	ParticleBase(ID3D11Device* device, int createType,std::string name);
	void Editor(std::vector<std::string>shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>texs);
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Draw(ID3D11DeviceContext* context);
	//getter
	std::string GetName() { return mName; }
	std::string GetFilePath() { return mFilePath; }
	const int GetDrowType() { return mDrowType; }
	const int GetTexNo() { return mTexNo; }

	~ParticleBase()
	{
		mCreate.reset();
		mMoves.clear();
	}
private:
	void CreateBuffer(ID3D11Device* device);
	void DrawEditorFunction(void* id, std::vector<std::string> shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texs);
	void ParticleMoveEdit(void* id);
	//変数
	UINT mMaxParticle;
	UINT mIndexCount;
	UINT mRenderCount;
	std::string mName;
	std::string mFilePath;
	int mDrowType;
	int mTexNo;
	int mMoveEditNo;
	int mMoveCreateTypeNo;
	std::vector<std::unique_ptr<ParticleMove>>mMoves;
	std::unique_ptr<ParticleCreateBase>mCreate;
	struct ParticleMoveParameter
	{
		float life;
		VECTOR3F accel;
		VECTOR3F anglerVelocity;
		VECTOR3F startPosition;
		float maxLife;

	};
	struct ParticleRenderParameter
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

};
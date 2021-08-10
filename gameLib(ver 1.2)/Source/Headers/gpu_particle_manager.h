#pragma once
#include"gpu_particle_test.h"
#include<vector>
#include<memory>
#include<string>
#include"constant_buffer.h"
#include"drow_shader.h"

class GpuParticleManager
{
public:
	GpuParticleManager(ID3D11Device* device);
	void Update(float elapsd_time);
	void Render(ID3D11DeviceContext* context);
#if (PARTICLE_SYSTEM_TYPE==0)
	~GpuParticleManager() { particles.clear(); }
private:

	class GpuParticleObj
	{

	public:
		GpuParticleObj(std::shared_ptr<GpuParticleTest>particle, std::string name)
		{
			mParticle = particle;
			mParticleName = name;
		}
		std::shared_ptr<GpuParticleTest>GetParticle() { return mParticle; }
		std::string GetParticleName() { return mParticleName; }
	private:
		std::shared_ptr<GpuParticleTest>mParticle;
		std::string mParticleName;
	};
	std::vector<GpuParticleObj>particles;
	int particleNimber;
#elif (PARTICLE_SYSTEM_TYPE==1)
	void Editor();
private:
	void PSSetTexture(ID3D11DeviceContext* context, GpuParticle*particle);
	std::vector<std::unique_ptr<GpuParticle>>mParticles;
	//std::unique_ptr<TitleParticle>mTitle;
	int mParticleNo;
	bool mEditorFlag;
	bool mCreateFlag;
	struct CreateEditorData
	{
		std::string name;
		int createType;
		void Reset()
		{
			name = "";
			createType = -1;
		}
	};
	CreateEditorData mCreateEditorData;
	std::vector<std::string>mTypeNameList;
	void CreateParticle(std::string name,int type);
	enum DrowType
	{
		Point,Board,Tex,Max
	};
	std::vector<std::string> mDrowTypeNames;
	struct CbTimer
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	std::unique_ptr<ConstantBuffer<CbTimer>>mCbTimer;
	std::unique_ptr<DrowShader>mShaders[DrowType::Max];
	struct TexData
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;

	};
	std::vector< Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mTexs;
#endif


};
static const char* drowObjType[] = { "" };

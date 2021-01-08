#pragma once
#include"static_mesh.h"
#include<vector>
#include"drow_shader.h"
#include"gpu_particle_test.h"

class GpuStaticMeshParticle:public GpuParticleTest
{
public:
	GpuStaticMeshParticle(ID3D11Device* device, const char* fileName);
	GpuStaticMeshParticle(ID3D11Device* device, StaticMesh* mesh);
	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	void CreateBuffer(ID3D11Device* device, StaticMesh* mesh);
	std::unique_ptr<StaticMesh>mMesh;
	std::unique_ptr<MeshRender>mRender;
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mMeshBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mIndexBuffer;
		int indexSize;

	};
	std::vector<Mesh>mMeshs;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDeactiveParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;

	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mUpdateShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mTexturte;
	int mMaxParticle;
	float mCreateTime;
	float mTimer;
	struct CbCreate
	{
		VECTOR3F color;
		int startIndex;
		FLOAT4X4 world;
	};
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F dummy;
	};

	struct RenderParticle
	{
		VECTOR4F position;
		VECTOR3F angle;
		VECTOR4F color;
		VECTOR3F velocity;
		VECTOR3F scale;
	};
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		VECTOR4F color;
		float life;
		float scale;
	};
	CbCreate mCbCreate;
};
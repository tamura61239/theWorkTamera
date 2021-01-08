#pragma once
#include"model.h"
#include<vector>
#include"drow_shader.h"
#include"gpu_particle_test.h"
#include"model_renderer.h"

class GpuSkinnedMeshParticle :public GpuParticleTest
{
public:
	GpuSkinnedMeshParticle(ID3D11Device* device, const char* fileName);
	GpuSkinnedMeshParticle(ID3D11Device* device, std::shared_ptr<ModelResource>& resource);
	void ImGuiUpdate();
	void Update(ID3D11DeviceContext* context, float elapsdTime);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	void CreateBuffer(ID3D11Device* device);
	struct Particle
	{
		VECTOR3F position;
		VECTOR3F velocity;
		float scale;
		float life;
		UINT color;
	};
	std::unique_ptr<Model>mModel;
	std::unique_ptr<ModelRenderer>mRender;
	float mEditorColor[4] = { 1,1,1,1 };
	struct RenderParticle
	{
		VECTOR4F position;
		float angle;
		UINT color;
		VECTOR3F velocity;
		float scale;
	};
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mMeshBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mIndexBuffer;
		int indexSize;
	};
	struct CbCreate
	{
		float startIndex;
		UINT color;
		float scale;
		float life;
		float speed;
		VECTOR3F dummy;
	};
	struct CbBone
	{
		FLOAT4X4 boneWorld[128];
	};
	struct CbUpdate
	{
		float elapsdTime;
		VECTOR3F dummy;
	};
	std::vector<Mesh>mMeshs;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDeactiveParticleUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mRenderUAV;

	Microsoft::WRL::ComPtr<ID3D11Buffer>mRenderBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbCreateBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBoneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbUpdateBuffer;

	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCreateShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mUpdateShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mTexturte;
	int mMaxParticle;
	float mCreateTime;
	float mTimer;
	CbCreate mCbCreate;
	int mAnimNo;
	bool mDrowModel;
};

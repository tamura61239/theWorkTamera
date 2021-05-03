#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"vector.h"
#include"drow_shader.h"
#include<memory>
#include<string>
#include"constant_buffer.h"
//�V���h�E�}�b�v�N���X
class RenderEffects
{
public:
	//�R���X�g���N�^
	RenderEffects(ID3D11Device* device,std::string fileName="");
	//�G�f�B�^
	void Editor();
	//�V�[���ɉe��t����
	void ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		,const FLOAT4X4&view,const FLOAT4X4&projection,const FLOAT4X4&lightView,const FLOAT4X4&lightProjection);
	void ShadowRender(ID3D11DeviceContext* context, DrowShader*shader,ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection);

private:
	//�G�f�B�^�ő��삵�ăZ�[�u����f�[�^
	struct SaveData
	{
		VECTOR3F shadowColor = VECTOR3F(0.65f, 0.65f, 0.65f);
		float shadowbisa = 0.0008f;
		float slopeScaledBias = 0;
		float depthBiasClamp = 0;
		VECTOR2F dummy;
	};
	//�萔�o�b�t�@
	struct CbScene
	{
		FLOAT4X4 inverseViewProjection;
		FLOAT4X4 lightViewProjection;
		SaveData saveData;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbBuffer;
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
	//�t�@�C���̖��O
	std::string mFileName = "";

};
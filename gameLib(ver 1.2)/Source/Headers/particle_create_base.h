#pragma once
#include<string>
#include<memory>
#include<d3d11.h>
#include<vector>
#include <wrl.h>

class ParticleCreateBase
{
public:
	//コンストラクタ
	ParticleCreateBase() = default;
	ParticleCreateBase(ID3D11Device* device) {}
	//ファイル操作ロード関数
	virtual void Load(std::string filePas) = 0;
	//エディタ関数
	virtual void Editor(void*id) =0;
	//パーティクルの生成関数
	virtual void CreateParticle(ID3D11DeviceContext* context, float elapsdTime) =0;

	virtual ~ParticleCreateBase() = default;
protected:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mShader;
};
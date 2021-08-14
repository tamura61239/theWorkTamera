#pragma once
#include"particle_create_base.h"
#include"constant_buffer.h"
#include"vector.h"
#include<string>

class ParticleCreatePoint :public ParticleCreateBase
{
public:
	//コンストラクタ
	ParticleCreatePoint(ID3D11Device* device);
	//ファイルデータのロード
	void Load(std::string filePas);
	//エディタ関数
	void Editor(void* id);
	//パーティク生成関数
	void CreateParticle(ID3D11DeviceContext* context, float elapsdTime);
	//getter
	//std::string GetFilePas() { return mFilePas; }
	~ParticleCreatePoint()
	{

	}
private:
	//GPU側に送信するデータ
	struct Point
	{
		VECTOR3F position;
		bool directionSpecifyFlag;
		VECTOR3F releaseDirection;
		float angleToFine;
	};
	struct Create
	{
		//Point point[15];
		int createCount;
		VECTOR3F dummy;
	};
	//定数バッファ
	std::unique_ptr<ConstantBuffer<VECTOR4F>>mCreateBuffer;
	//エディタで操作するデータ
	struct EditData
	{
		Point point[15];
		int createCount;
		int pointCount;
	};
	EditData mEditData;
	std::string mFilePas;
	float mTotalCreateCount;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mShader;

};
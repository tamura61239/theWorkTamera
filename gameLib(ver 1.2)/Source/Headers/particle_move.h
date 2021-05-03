#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"vector.h"
#include<string>
#include"constant_buffer.h"
#include<memory>


class ParticleMove
{
public:
	ParticleMove(ID3D11Device* device, std::string fileName = "");
	virtual void Editor();
	virtual void Move(ID3D11DeviceContext* context, UINT x);
	const std::string GetMoveName() { return mName; }
	const std::string GetFileName() { return mFileName; }
private:
	struct MoveData
	{
		VECTOR4F endColor;
		float maxSpeed;
		float colorChangeTime;
		VECTOR2F dummy;
	};
	std::unique_ptr<ConstantBuffer<MoveData>>mCBuffer;
protected:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mShader;
	std::string mName;
	std::string mFileName;
};
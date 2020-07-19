#pragma once
#include"gpu_particle_test.h"

class GpuNoiseGeometricParticle :public GpuParticleTest
{
public:
	GpuNoiseGeometricParticle(ID3D11Device* device, const int maxParticle=10000);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	struct VertexGeometric
	{
		VECTOR4F position;
		VECTOR4F color;
		VECTOR3F angle;
		VECTOR3F velocity;
		float life;
		float maxLife;
	};
	struct CbMoveDataBuffer
	{
		VECTOR3F standardVelocity;
		float dummy2;
		VECTOR3F angleSpeed;
		float dummy3;
		VECTOR3F startPosition;
		float dummy4;
	};
	struct CbStartBuffer
	{
		float startIndex;
		float indexSize;
		float setLife;
		float timer;
		VECTOR4F startColor;
	};
	CbMoveDataBuffer cbMoveDataBuffer;
	CbStartBuffer cbStartBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
};
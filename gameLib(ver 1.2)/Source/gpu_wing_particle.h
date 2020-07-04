#pragma once
#include"gpu_particle_test.h"

class GpuWingParticle :public GpuParticleTest
{
public:
	GpuWingParticle(ID3D11Device* device, const int maxParticle = 10000, const VECTOR3F& centerPosition = VECTOR3F(0, 0, 0));
	void Update(ID3D11DeviceContext* context);
private:
	struct VertexWing
	{
		VECTOR4F position;
		VECTOR4F color;
		VECTOR3F velocity;
		float wingCount;

	};
	struct VertexStartParrametr
	{
		float wingCount;
		float length;
		VECTOR4F position;
	};
	struct WingEndPoint
	{
		VECTOR4F position;
		VECTOR4F color;
		VECTOR4F size;
	};
	struct ConstanceWing
	{
		WingEndPoint wingEndPoint[4];
		VECTOR3F centerPosition;
		float startIndex;
		float indexSize;
		VECTOR3F dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mResetUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mResetBuffer;

	ConstanceWing mConstanceWing;
};
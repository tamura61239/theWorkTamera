#pragma once
#include"gpu_particle_test.h"

#if (PARTICLE_SYSTEM_TYPE==0)

class GpuCloudParticle :public GpuParticleTest
{
public:
	GpuCloudParticle(ID3D11Device* device, int maxParticle = 10000, const VECTOR2F& maxLength = VECTOR2F(1000, 1000),
		const VECTOR3F& cloudDirection = VECTOR3F(-1, 0, 0), const VECTOR3F& startPosition = VECTOR3F(0, 0, 0));
	void Update(ID3D11DeviceContext* context, float elapsd_time);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
private:
	struct VertexCloud
	{
		VECTOR4F position;
		float cloudMass;
		float cloudSize;
		VECTOR4F color;
	};
	struct ComputeComstance
	{
		//VECTOR3F windVelocity;
		//VECTOR2F length;
		//float startIndex;
		//float indexSize;
		//float dummy;
		float startIndex;
		float indexSize;
		VECTOR2F length;
		VECTOR3F windVelocity;
		float windPower;
	};
	struct Swirl
	{
		VECTOR3F centerPosition;
		float length;
		float flag;
		VECTOR3F dummy;
	};
	struct SwirlComstance
	{
		Swirl swirls[10];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mResetBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mSwirlBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mResetUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	ComputeComstance mConputeComstance;
	SwirlComstance mSwirlComstance;
};
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
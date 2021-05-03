#pragma once
#include"gpu_particle_test.h"

#if (PARTICLE_SYSTEM_TYPE==0)

class GpuCurlNoiseParticle :public GpuParticleTest
{
public:
	GpuCurlNoiseParticle(ID3D11Device* device, const int maxParticle = 100000);
	void Update(ID3D11DeviceContext* context, float elapsd_time);
private:
	struct VertexCurlNoise
	{
		VECTOR4F position;
		VECTOR3F velocity;
		VECTOR4F color;
		float life;
	};
	struct CurlNoise
	{
		VECTOR3F startPosition;
		float timer;
		VECTOR4F startColor;
		VECTOR4F endColor;
	};
	struct NoiseParameter
	{
		float maxParticle;
		float speed;
		float noizeNormalizeFlag;
		float noiseSize;
		VECTOR3F vec0;
		VECTOR3F vec1;
		VECTOR2F dummy;
	};
	struct Parameter
	{
		float randSpeed;
		bool defSpeedFlag;
		float defSpeed;
	};
	CurlNoise mCurlNoise;
	NoiseParameter mNoiseParameter;
	Parameter mParameter;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mNoiseBuffer;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
};
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
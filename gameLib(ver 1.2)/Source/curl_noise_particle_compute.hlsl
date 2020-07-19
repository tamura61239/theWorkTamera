#include"curl_noise.hlsli"
RWByteAddressBuffer rwBuffer:register(u0);
cbuffer ConputeConstance:register(b0)
{
	float3 startPosition;
	float timer;
	float4 startColor;
	float4 endColor;
}
cbuffer NoiseParameter:register(b1)
{
	float maxParticle;
	float speed;
	float noizeNormalizeFlag;
	float noiseSize;
	float3 vec0;
	float3 vec1;
	float2 dummy;
}
float3 curl(in float3 p, in float noiseTime, in float persistence) {

	float3 xNoisePotentialDerivatives = float3(0,0,0);
	float3 yNoisePotentialDerivatives = float3(0,0,0);
	float3 zNoisePotentialDerivatives = float3(0,0,0);

	for (int i = 0; i < 3; ++i) {

		float twoPowI = pow(2.0, float(i));
		float scale = 0.5 * twoPowI * pow(persistence, float(i));

		xNoisePotentialDerivatives += lerp(snoise(float3(p * twoPowI)*noiseTime * noiseSize),normalize(snoise(float3(p * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
		yNoisePotentialDerivatives += lerp(snoise(float3((p + vec0) * twoPowI) * noiseTime * noiseSize), normalize(snoise(float3((p + vec0) * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
		zNoisePotentialDerivatives += lerp(snoise(float3((p + vec1) * twoPowI)* noiseTime * noiseSize), normalize(snoise(float3((p + vec1) * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
	}

	return float3(
		zNoisePotentialDerivatives.y - yNoisePotentialDerivatives.z,
		xNoisePotentialDerivatives.z - zNoisePotentialDerivatives.x,
		yNoisePotentialDerivatives.x - xNoisePotentialDerivatives.y
	);

}

float3 curlNoise(float3 p) {
	float3 noise0 = snoise(p);
	float3 noise1 = snoise(vec0);
	float3 noise2 = snoise(vec1);
	return normalize(noise0 + noise1 + noise2);
}
float4 SetPosition(uint dtid)
{
	float d = 1.0f / (float)(randXorshift(maxParticle));
	//float z = (float)(randXorshift(dtid)) * d-0.5f;
	//float angle = radians(z * 360)- radians(180);
	//float x = sqrt(1 - z * z) * cos(angle);
	//float y = sqrt(1 - z * z) * sin(angle);
	//float d = 1.0f / 4294967296.0f;
	float f0 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f1 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f2 = (float)(randXorshift(dtid)) * d - 0.5f;
	float3 normalF3 = normalize(float3(f0, f1, f2)) * 0.2f;
	normalF3 *= (float)(randXorshift(dtid)) * d * 0.01f;
	float3 position = float3(normalF3.x + startPosition.x, normalF3.y + startPosition.y, normalF3.z + startPosition.z);
	return float4(position * 0.1f, 1);
	//return float4(x, y, z, 1);
}
float4 SetPosition(float2 uv)
{
	float x = nrand(uv);
	float y = nrand(uv);
	float z = nrand(uv);
	float3 position = normalize(float3(x, y, z));
	return float4(position, 1);
}
[numthreads(100, 1, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.z + DTid.y + DTid.x;
	uint bufferIndex = index * 48;
	float4 position = asfloat(rwBuffer.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwBuffer.Load3(bufferIndex + 16));
	float4 color = asfloat(rwBuffer.Load4(bufferIndex + 28));
	float life = asfloat(rwBuffer.Load(bufferIndex + 44));
	life -= timer;
	velocity += lerp(snoise(position.xyz - startPosition) * noiseSize, normalize(snoise(position.xyz - startPosition)), noizeNormalizeFlag);
	//velocity += curl(position.xyz - startPosition, speed,0.4f);
	//velocity += curl(snoise(position.xyz - startPosition)* noiseSize, speed,0.4f);
	//velocity += lerp(curlNoise(position.xyz-startPosition),normalize(curlNoise(position.xyz - startPosition)), noizeNormalizeFlag);
	position.xyz += velocity*speed;
	//‰Šú‰»
	float set = step(0, life);
	position = lerp(SetPosition(DTid.x), position, set);
	velocity = lerp(normalize(startPosition-position.xyz), velocity, set);
	life = lerp(2, life, set);
	color = lerp(endColor, startColor, max(life / 2, 0));
	//color.a += 1.f;
	rwBuffer.Store4(bufferIndex + 0, asuint(position));
	rwBuffer.Store3(bufferIndex + 16, asuint(velocity));
	rwBuffer.Store4(bufferIndex + 28, asuint(color));
	rwBuffer.Store(bufferIndex + 44, asuint(life));
}

#include"curl_noise.hlsli"
RWByteAddressBuffer rwBuffer:register(u0);
cbuffer ConputeConstance:register(b0)
{
	float3 startPosition;
	float timer;
	float4 startColor;
	float4 endColor;
}
float4 SetPosition(uint dtid)
{
	float d = 1.0f / 4294967296.0f;
	float f0 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f1 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f2 = (float)(randXorshift(dtid)) * d - 0.5f;
	float3 normalF3 = normalize(float3(f0, f1, f2)) * 0.2f;
	normalF3 *= (float)(randXorshift(dtid)) * d*0.01f;
	float3 position = float3(normalF3.x + startPosition.x, normalF3.y + startPosition.y, normalF3.z + startPosition.z);
	return float4(position*0.1f, 1);
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
	velocity = curlNoise(position.xyz);
	position.xyz += velocity;
	float set = step(0, life);
	position = lerp(SetPosition(DTid.x), position, set);
	velocity = lerp(float3(0, 0, 0), velocity, set);
	life = lerp(5, life, set);
	color = float4(1,1,1,1);
	rwBuffer.Store4(bufferIndex + 0, asuint(position));
	rwBuffer.Store3(bufferIndex + 16, asuint(velocity));
	rwBuffer.Store4(bufferIndex + 28, asuint(color));
	rwBuffer.Store(bufferIndex + 44, asuint(life));
}

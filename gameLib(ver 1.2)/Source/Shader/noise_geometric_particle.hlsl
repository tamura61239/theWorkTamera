#include"curl_noise.hlsli"
RWByteAddressBuffer rwBuffer:register(u0);
cbuffer CbMoveDataBuffer:register(b0)
{
	float3 standardVelocity;
	float dummy2;
	float3 angleSpeed;
	float dummy3;
	float3 startPosition;
	float dummy4;
}
cbuffer CbStartBuffer:register(b1)
{
	float startIndex;
	float indexSize;
	float setLife;
	float timer;
	float4 startColor;
}
float3 SetPosition(uint index)
{
	float angle = index % 360;
	float x = sin(angle);
	float z = cos(angle);
	float y = x * z;
	return startPosition+float3(x, y, z);
}
[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.z + DTid.y + DTid.x;
	uint bufferIndex = index * 64;
	//ì«Ç›çûÇ›
	float4 position = asfloat(rwBuffer.Load4(bufferIndex + 0));
	float4 color = asfloat(rwBuffer.Load4(bufferIndex + 16));
	float3 angle = asfloat(rwBuffer.Load3(bufferIndex + 32));
	float3 velocity = asfloat(rwBuffer.Load3(bufferIndex + 44));
	float life = asfloat(rwBuffer.Load(bufferIndex + 56));
	float maxLife = asfloat(rwBuffer.Load(bufferIndex + 60));

	//èâä˙âª
	if (startIndex<=index)
	{
		if (index < startIndex + indexSize&&life>=maxLife)
		{
			position.xyz = SetPosition(index);
			position.w = 1;
			velocity = normalize(position.xyz - startPosition) * 0.01f;
			color = startColor;
			life = 0;
			maxLife = setLife;
			angle = float3(0, 0, 0);
		}
	}
	//position.xyz = lerp(position.xyz, SetPosition(index), start);
	life += timer;
	float dstroy = step(maxLife, life);
	//velocity = lerp(velocity, standardVelocity, 0.3f);
	velocity += float3(normalize(snoise(position.xyz-startPosition))+ standardVelocity)*timer;
	position.xyz = lerp(position.xyz + velocity, float3(0, 0, 0), dstroy);
	color = lerp(color, float4(0, 0, 0,0), dstroy);
	angle = lerp(angle + angleSpeed * timer, float3(0, 0, 0), dstroy);
	//position.xyz = float3(0, 0, 0);
	//èëÇ´çûÇ›
	rwBuffer.Store4(bufferIndex + 0, asuint(position));
	rwBuffer.Store4(bufferIndex + 16, asuint(color));
	rwBuffer.Store3(bufferIndex + 32, asuint(angle));
	rwBuffer.Store3(bufferIndex + 44, asuint(velocity));
	rwBuffer.Store(bufferIndex + 56, asuint(life));
	rwBuffer.Store(bufferIndex + 60, asuint(maxLife));
}
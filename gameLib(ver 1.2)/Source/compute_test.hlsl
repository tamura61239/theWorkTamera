RWByteAddressBuffer		rwbParticle : register(u0);

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	// パーティクルの通し番号を求める
	uint	index = DTid.z * 100 * 1 + DTid.y * 100 + DTid.x;

	uint bufferIndex = index * 44;
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 16));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 28));
	position.xyz += velocity;
	//if (position.x > 10 || position.x < -10)
	//{
	//	if (position.y > 10 || position.y < -10)
	//	{
	//		if (position.z > 10 || position.z < -10)
	//		{
	//			position = float4(0, 0, 0, 1);
	//		}
	//	}
	//}
	float3 regionFlag;
	regionFlag.x = 1.0f-step(position.x, 10) * step(-10, position.x);
	regionFlag.y = 1.0f-step(position.y, 10) * step(-10, position.y);
	regionFlag.z = 1.0f-step(position.z, 10) * step(-10, position.z);
	//if (position.x > 10 || position.x < -10)position.x = velocity.x;
	//if (position.y > 10 || position.y < -10)position.y = velocity.y;
	//if (position.z > 10 || position.z < -10)position.z = velocity.z;
	position.x = lerp(position.x, velocity.x, regionFlag.x);
	position.y = lerp(position.y, velocity.y, regionFlag.y);
	position.z = lerp(position.z, velocity.z, regionFlag.z);
	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store3(bufferIndex + 16, asuint(velocity));
	rwbParticle.Store4(bufferIndex + 28, asuint(color));
}
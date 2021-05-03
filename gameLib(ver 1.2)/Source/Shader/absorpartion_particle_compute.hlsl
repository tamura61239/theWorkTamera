cbuffer Comstance:register(b0)
{
	float3 centerPosition;
	float size;
}

RWByteAddressBuffer		rwbParticle : register(u0);
RWByteAddressBuffer		resetParticle : register(u1);

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.z + DTid.y + DTid.x;

	uint bufferIndex = index * 44;
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 16));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 28));

	float3 vec = centerPosition - position.xyz;
	float leng = length(vec);
	vec = normalize(vec);
	float3 up = float3(0, 1, 0);
	//leng = 1 - 1 / leng;
	//移動方向を求める
	velocity = cross(up, vec);
	velocity *= 0.01f;
	position.xyz += velocity + vec * 0.03f;
	//中心からの距離が一定距離より短くなったら初期化する
	uint set = step(0.1f, leng);
	position = lerp(asfloat(resetParticle.Load4(bufferIndex + 0)), position, set);
	color = lerp(asfloat(resetParticle.Load4(bufferIndex + 28)), color, set);
	vec = centerPosition - position.xyz;
	leng = length(vec);
	//色や不透明度を求める
	float l = leng - 0.4f* size;
	l /= 0.1f * size;
	l = max(l, 0);
	l = min(l, 1);
	color.rgb = lerp(float3(1, 0.5, 0), float3(0, 1, 1), l);
	l = leng - 0.7f* size;
	l /= 3.f;
	l = max(l, 0);
	l = min(l, 1);
	color.a = lerp(1, 0, l);

	
	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store3(bufferIndex + 16, asuint(velocity));
	rwbParticle.Store4(bufferIndex + 28, asuint(color));

}
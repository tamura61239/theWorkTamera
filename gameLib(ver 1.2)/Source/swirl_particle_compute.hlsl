#include"swirl.hlsli"

RWByteAddressBuffer		rwbParticle : register(u0);
RWByteAddressBuffer		resetParticle : register(u1);

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint	index = DTid.z * 100 * 1 + DTid.y * 100 + DTid.x;

	uint bufferIndex = index * 44;
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 16));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 28));

	//velocity.x = sin(velocity.y);
	//velocity.z = cos(velocity.y);
	//float3 vec = normalize(position.xyz - centerPosition)*0.001f;
	//velocity.x -= vec.x;
	//velocity.z -= vec.z;
	//position.x += velocity.x*0.001f;
	//position.z += velocity.z*0.001f;
	//velocity.y += 0.01745f;
	float3 vec = centerPosition - position.xyz;
	float leng = length(vec);
	vec = normalize(vec);
	float3 up = float3(0, 1, 0);
	//leng = 1 - 1 / leng;
	velocity = cross(up, vec);
	velocity += vec;
	velocity = normalize(velocity);
	velocity *= 0.01f/* * leng*/;
	position.xyz += velocity;
	//step(a,x):(x >= a) ? 1 : 0 (xÇ™aà»è„ÇÃèÍçáÇÕ1, ÇªÇÍà»äOÇÕ0Çï‘ÇµÇ‹Ç∑)
	uint set = step(0.01f, leng);
	position = lerp(asfloat(resetParticle.Load4(bufferIndex + 0)), position, set);
	color = lerp(asfloat(resetParticle.Load4(bufferIndex + 28)), color, set);
	float l = leng - 4.f;
	l = max(l,0);
	l = min(l, 1);
	color.rgb = lerp(float3(1, 0.5, 0), float3(0, 1, 1), l);
	l = leng - 7.0f;
	l /= 3.f;
	l = max(l, 0);
	l = min(l, 1);
	color.w = lerp(1, 0, l);
	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store3(bufferIndex + 16, asuint(velocity));
	rwbParticle.Store4(bufferIndex + 28, asuint(color));

}
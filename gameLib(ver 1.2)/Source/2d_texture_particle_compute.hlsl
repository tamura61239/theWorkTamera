RWByteAddressBuffer		rwbParticle : register(u0);

[numthreads(10, 10, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint	index = DTid.z * 100 * 1 + DTid.y * 100 + DTid.x;

	uint bufferIndex = index * 44;
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 16));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 28));

	float2 vec = normalize(position.xy - float2(0, 0));
	float3 c = cross(float3(vec, 0), float3(0, -1, 0));
	vec *= c.z;
	float d = dot(vec, float2(0, -1));
	float angle = acos(d);
	angle += 0.01745;
	d = cos(angle);
	float s = sin(angle);
	position.xy += velocity.xy;
	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store3(bufferIndex + 16, asuint(velocity));
	rwbParticle.Store4(bufferIndex + 28, asuint(color));

}
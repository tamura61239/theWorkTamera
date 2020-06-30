RWByteAddressBuffer rwParticle:register(u0);
RWByteAddressBuffer resetParticle:register(u1);
cbuffer ConputeComstance:register(b0)
{
	float startIndex;
	float indexSize;
	float2 maxLength;
	float3 windVelocity;
	float windPower;
}
struct Swirl
{
	float3 centerPosition;
	float swirlLength;
	float flag;
	float3 swirlDummy;
};
cbuffer SwirlConstance:register(b1)
{
	Swirl swirl[10];
}
[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint	index = DTid.z  + DTid.y + DTid.x;

	uint bufferIndex = index * 44;
	uint resetBufferIndex = index * 40;
	float4 position = asfloat(rwParticle.Load4(bufferIndex + 0));
	float4 color = asfloat(rwParticle.Load4(bufferIndex + 28));
	//‰Šú‰»‚·‚é’l
	float4 startPosition = asfloat(resetParticle.Load4(resetBufferIndex + 0));
	float4 startColor = asfloat(resetParticle.Load4(resetBufferIndex + 24));
	float start = max(step(startIndex, index) - step(startIndex + indexSize, index), 0);
	//‰Šú‰»
	//if (index >= startIndex && index < startIndex + indexSize)
	//{
	//	position = startPosition;
	//	color = float4(1,1,1,1);
	//	color.a = 1;
	//}
	position = lerp(position, startPosition, start);
	color = lerp( color,startColor, start);
	float mass = asfloat(resetParticle.Load(resetBufferIndex + 16));
	float size = asfloat(resetParticle.Load(resetBufferIndex + 20));

	float density = mass / size;
	float3 velocity = windVelocity / density;
	position.xyz += velocity;
	color.a = lerp(color.a, size/mass, 0.1f);
	color.r = size / mass;
	color.g = 0;
	color.b = density;
	color.rgb = normalize(color.rgb);
	//‰QŠª
	uint set = 0;
	for (int i = 0;i < 10;i++)
	{
		float3 vec = swirl[i].centerPosition - position.xyz;
		float leng = length(vec);
		vec = normalize(vec);
		float3 up = float3(0, 1, 0);
		float3 swirlVec = cross(up, vec);
		float3 swirlVelocity = lerp(float3(0, 0, 0), swirlVec + vec, step(leng, swirl[i].swirlLength));
		swirlVelocity /= density;
		position.xyz += swirlVelocity * swirl[i].flag;
		set = step(leng, 1.0f) * swirl[i].flag;
		position = lerp(position, startPosition, set);
		color = lerp(color,startColor , set);
	}

	float x = step(maxLength.x,length(startPosition.x - position.x));
	float z = step(maxLength.y,length(startPosition.z - position.z));

	set = min(x + z, 1);

	position = lerp(position,startPosition , set);
	color = lerp(color, startColor, set);

	rwParticle.Store4(bufferIndex + 0, asuint(position));
	rwParticle.Store4(bufferIndex + 28, asuint(color));

}
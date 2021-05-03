RWByteAddressBuffer		rwbParticle : register(u0);//更新する値
RWByteAddressBuffer		resetParticle : register(u1);//初期値

struct WingEndPoint
{
	float4 position;
	float4 color;
	float4 size;
};

cbuffer ConstanceWing:register(b0)
{
	WingEndPoint wingEndPoint[4];
	float3 centerPosition;
	float startIndex;
	float indexSize;
	float3 dummy;

}
[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint	index = DTid.z * 100 * 1 + DTid.y * 100 + DTid.x;
	uint bufferIndex = index * 48;
	uint resetBufferIndex = index * 24;
	//rwbParticleから値を取得
	float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	float4 color = asfloat(rwbParticle.Load4(bufferIndex + 16));
	float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 32));
	float wingCount = asfloat(rwbParticle.Load(bufferIndex + 44));
	//resetParticleから値を取得
	float startWingCount = asfloat(resetParticle.Load(resetBufferIndex + 0));
	float startLength = asfloat(resetParticle.Load(resetBufferIndex + 4));
	float4 startRangePosition = asfloat(resetParticle.Load4(resetBufferIndex + 8));
	//初期位置算出
	float startCount = step(2, startWingCount);
	float3 startMinSize = lerp(float3(0, 0, 0), wingEndPoint[startWingCount - 2].size.xyz, startCount);
	float3 startSize = lerp(startMinSize, wingEndPoint[startWingCount].size.xyz, startLength);
	float3 startMinPosition = lerp(centerPosition, wingEndPoint[startWingCount - 2].position.xyz, startCount);
	float3 startMaxPosition = wingEndPoint[startWingCount].position.xyz;
	float wingLeng = length(startMaxPosition.xyz - startMinPosition.xyz);
	float4 startPosition = wingEndPoint[startWingCount].position + float4(normalize(startMinPosition- startMaxPosition), 0) * startLength* wingLeng + float4(startRangePosition.xyz * startSize, 0);
	float maxLength = lerp(length(startMinPosition - startPosition.xyz), length(startMaxPosition - startPosition.xyz), startCount);

	//初期化
	float start = max(step(startIndex, index) - step(startIndex + indexSize, index), 0);
	position = lerp(position, startPosition, start);
	color = lerp(color, float4(1,1,1,1), start);
	wingCount = lerp(wingCount, startWingCount, start);
	velocity = lerp(velocity, float3(0, 0, -1), start);
	//更新
	position.xyz += velocity * 0.05f;
	float leng = length(startPosition.xyz - position.xyz);
	//color.a = lerp(1, 0, leng / maxLength);
	//リセット
	float reset = step(maxLength, startPosition.z-position.z);
	position = lerp(position, startPosition, reset);
	color = lerp(color, float4(1,1,1,1), reset);
	wingCount = lerp(wingCount, startWingCount, reset);
	velocity = lerp(velocity, float3(0, 0, -1), reset);

	//uint bufferIndex = index * 48;
	//RWByteAddressBufferから値を取得
	//float4 position = asfloat(rwbParticle.Load4(bufferIndex + 0));
	//float4 color = asfloat(rwbParticle.Load4(bufferIndex + 16));
	//float3 velocity = asfloat(rwbParticle.Load3(bufferIndex + 32));
	//float wingCount = asfloat(rwbParticle.Load(bufferIndex + 44));
	////初期値
	//float4 startPosition = asfloat(resetParticle.Load4(bufferIndex + 0));
	//float4 startColor = asfloat(resetParticle.Load4(bufferIndex + 16));
	//float startWingCount = asfloat(resetParticle.Load(bufferIndex + 44));
	////初期化
	//float start = max(step(startIndex, index) - step(startIndex + indexSize, index), 0);
	//position = lerp(position, startPosition, start);
	//color = lerp(color, startColor, start);
	//wingCount = lerp(wingCount, startWingCount, start);
	////更新
	//float state1 = step(2, wingCount);
	//float state2 = step(4, wingCount);
	//velocity = lerp(normalize(wingEndPoint[wingCount].position.xyz-position.xyz), velocity, state2);
	//float3 vec = normalize(position.xyz - wingEndPoint[wingCount].position.xyz);
	//float leng = length(wingEndPoint[wingCount].position.xyz - position);
	//float maxLeng = length(wingEndPoint[wingCount].position.xyz - centerPosition);
	////color.rgb = lerp(lerp(wingEndPoint[wingCount].color.rgb, color, max(leng / maxLeng,0)), color.rgb, state2);
	////color.a -= lerp(0, 0.1f, state2);
	//
	//float d = dot(vec, velocity);
	//wingCount += lerp(0, 2, step(0.1, d));
	//position.xyz += velocity;

	//float resert = 1.f-step(0, color.a);
	//position = lerp(position, startPosition, state2);
	//color = lerp(color, startColor, state2);
	//wingCount = lerp(wingCount, startWingCount, state2);

	rwbParticle.Store4(bufferIndex + 0, asuint(position));
	rwbParticle.Store4(bufferIndex + 16, asuint(color));
	rwbParticle.Store3(bufferIndex + 32, asuint(velocity));
	rwbParticle.Store(bufferIndex + 44, asuint(wingCount));
}
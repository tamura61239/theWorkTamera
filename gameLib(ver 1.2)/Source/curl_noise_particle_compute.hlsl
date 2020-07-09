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
}
float4 grad4(float j, float4 ip)
{
	float4 ones = float4(1, 1, 1, -1);
	float4 p, s;
	p.xyz = floor(frac(float3(j,j,j)*ip.xyz) * 7) * ip.z - 1;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	//s = float4(lessThan(p, float4(0)));
	//step(a,x):(x >= a) ? 1 : 0 (xÇ™aà»è„ÇÃèÍçáÇÕ1, ÇªÇÍà»äOÇÕ0Çï‘ÇµÇ‹Ç∑)
	s.x = lerp(p.x,0 , step(p.x, 0));
	s.y = lerp(p.y,0 , step(p.y, 0));
	s.z = lerp(p.z,0 , step(p.z, 0));
	s.w = lerp(p.w,0 , step(p.w, 0));
	p.xyz = p.xyz + (s.xyz * 2 - 1) * s.www;
	return p;
}
float3 snoise(float3 vec)
{
	float4 v = float4(vec, 1);
	float4 c = float4(0.138196601125011, 0.276393202250021, 0.414589803375032, -0.447213595499958);
	float4 i = floor(v + dot(v, float4(0.309016994374947451, 0.309016994374947451, 0.309016994374947451, 0.309016994374947451)));
	float4 x0 = v - i + dot(i, c.xxxx);

	float4 i0;
	float3 isX = step(x0.yzw, x0.xxx);
	float3 isYZ = step(x0.zww, x0.yyz);
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0f - isX;
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0f - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0f - isYZ.z;

	float4 i3 = saturate(i0);
	float4 i2 = saturate(i0 - 1);
	float4 i1 = saturate(i0 - 2);

	float4 x1 = x0 - i1 + c.xxxx;
	float4 x2 = x0 - i2 + c.yyyy;
	float4 x3 = x0 - i3 + c.zzzz;
	float4 x4 = x0 + c.wwww;

	i = mod(i, 289);
	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	float4 j1 = permute(permute(permute(permute(
		i.w + float4(i1.w, i2.w, i3.w, 1.0f))
		+ i.z + float4(i1.z, i2.z, i3.z, 1.0f))
		+ i.y + float4(i1.y, i2.y, i3.y, 1.0f))
		+ i.x + float4(i1.x, i2.x, i3.x, 1.0f));

	float4 ip = float4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	float4 p0 = grad4(j0, ip);
	float4 p1 = grad4(j1.x, ip);
	float4 p2 = grad4(j1.y, ip);
	float4 p3 = grad4(j1.z, ip);
	float4 p4 = grad4(j1.w, ip);

	float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4, p4));

	float3 values0 = float3(dot(p0, x0), dot(p1, x1), dot(p2, x2));
	float2 values1 = float2(dot(p3, x3), dot(p4, x4));

	float3 m0 = max(0.5f - float3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0);
	float2 m1 = max(0.5f - float2(dot(x3, x3), dot(x4, x4)), 0);

	float3 temp0 = -6 * m0 * m0 * values0;
	float2 temp1 = -6 * m1 * m1 * values1;

	float3 mmm0 = m0 * m0 * m0;
	float2 mmm1 = m1 * m1 * m1;

	float dx = temp0.x * x0.x + temp0.y * x1.x + temp0.z * x2.x 
		+ temp1.x * x3.x + temp1.y * x4.x + mmm0.x * p1.x 
		+ mmm0.y * p2.x + mmm1.x * p3.x + mmm1.y * p4.x;
	float dy = temp0.x * x0.y + temp0.y * x1.y + temp0.z * x2.y
		+ temp1.x * x3.y + temp1.y * x4.y + mmm0.x * p1.y
		+ mmm0.y * p2.y + mmm1.x * p3.y + mmm1.y * p4.y;
	float dz = temp0.x * x0.z + temp0.y * x1.z + temp0.z * x2.z
		+ temp1.x * x3.z + temp1.y * x4.z + mmm0.x * p1.z
		+ mmm0.y * p2.z + mmm1.x * p3.z + mmm1.y * p4.z;
	float dw = temp0.x * x0.w + temp0.y * x1.w + temp0.z * x2.w
		+ temp1.x * x3.w + temp1.y * x4.w + mmm0.x * p1.w
		+ mmm0.y * p2.w + mmm1.x * p3.w + mmm1.y * p4.w;
	return normalize(float3(dx, dy, dz)) * noiseSize * dw;
}

float4 SetPosition(uint dtid)
{
	float d = 1.0f / (float)(randXorshift(maxParticle));
	//float d = 1.0f / 4294967296.0f;
	float f0 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f1 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f2 = (float)(randXorshift(dtid)) * d - 0.5f;
	float3 normalF3 = normalize(float3(f0, f1, f2)) * 0.2f;
	normalF3 *= (float)(randXorshift(dtid)) * d * 0.01f;
	float3 position = float3(normalF3.x + startPosition.x, normalF3.y + startPosition.y, normalF3.z + startPosition.z);
	return float4(position * 0.1f, 1);
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
	velocity += lerp(snoise(position.xyz),normalize(snoise(position.xyz)), noizeNormalizeFlag);
	position.xyz += velocity* speed;
	//èâä˙âª
	float set = step(0, life);
	position = lerp(SetPosition(DTid.x), position, set);
	velocity = lerp(normalize(startPosition-position.xyz), velocity, set);
	life = lerp(2, life, set);
	color = lerp(endColor, startColor, max(life / 2, 0));
	rwBuffer.Store4(bufferIndex + 0, asuint(position));
	rwBuffer.Store3(bufferIndex + 16, asuint(velocity));
	rwBuffer.Store4(bufferIndex + 28, asuint(color));
	rwBuffer.Store(bufferIndex + 44, asuint(life));
}

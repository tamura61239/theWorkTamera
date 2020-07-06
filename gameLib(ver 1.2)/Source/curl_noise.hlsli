//aÇÃnÇ…ÇÊÇÈèËó]
float mod(float a, float n)
{
	return a - floor(a / n) * n;
}
float2 mod(float2 a, float n)
{
	return a - floor(a / n) * n;
}
float3 mod(float3 a, float n)
{
	return a - floor(a / n) * n;
}
float4 mod(float4 a, float n)
{
	return a - floor(a / n) * n;
}

float4 permute(float4 a)
{
	return mod((a * 34.0 + 1.0) * a,289);
}
float4 taylorInvSqrt(float4 r)
{
	return 1.79284291400159f - r * 0.85373472095314f;
}
float snoise(float3 v)
{
	const float2 c = float2(1.0f / 6.0f, 1.0f / 3.0f);

	float3 i = floor(v + dot(v, c.yyy));
	float3 x0 = v - i + dot(i, c.xxx);

	float3 g = step(x0.yzx, x0.xyz);
	float3 l = 1.0f - g;
	float3 iMin = min(g.xyz, l.zyx);
	float3 iMax = max(g.xyz, l.zyx);

	float3 x1 = x0 - iMin + c.xxx;
	float3 x2 = x0 - iMax + c.yyy;
	float3 x3 = x0 - 0.5f;

	//permutations
	i = mod(i, 289);
	float4 p = permute(permute(permute(i.z + float4(0, iMin.z, iMax.z, 1))
		+ i.y + float4(0, iMin.y, iMax.y, 1))
		+ i.x + float4(0, iMin.x, iMax.x, 1));
	float4 j = mod(p, 49);

	float4 x_ = floor(j / 7);
	float4 y_ = floor(j - 7 * x_);

	float4 x = (x_ * 2 + 0.5f) / 7 - 1;
	float4 y = (y_ * 2 + 0.5f) / 7 - 1;

	float4 h = 1 - abs(x) - abs(y);

	float4 b0 = float4(x.xy, y.xy);
	float4 b1 = float4(x.zw, y.zw);

	float4 s0 = floor(b0) * 2 + 1;
	float4 s1 = floor(b1) * 2 + 1;
	float4 sh = -step(h, 0);

	float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	float3 g0 = float3(a0.xy, h.x);
	float3 g1 = float3(a0.zw, h.y);
	float3 g2 = float3(a1.xy, h.z);
	float3 g3 = float3(a1.zw, h.w);

	//Normalize gradients
	float4 norm = taylorInvSqrt(float4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
	g0 *= norm.x;
	g1 *= norm.y;
	g2 *= norm.z;
	g3 *= norm.w;

	//Min final noise value
	float4 m = max(0.6f - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0);
	m = m * m;
	m = m * m;

	float4 px = float4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
	return 42 * dot(m, px);
}
float nrand(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}
uint randXorshift(uint rngState)
{
	rngState ^= (rngState << 13);
	rngState ^= (rngState << 17);
	rngState ^= (rngState << 5);
	return rngState;
}
float3 snoiseVec3(float3 x)
{
	float s = snoise(x);
	float s1 = snoise(float3(x.y - 19.1f, x.z + 33.4f, x.x + 47.2f));
	float s2 = snoise(float3(x.z - 19.1f, x.x + 33.4f, x.y + 47.2f));
	float3 c = float3(s, s1, s2);
	return c;
}
float3 curlNoise(float3 p)
{
	const float e = 0.1f;
	float3 dx = float3(e, 0, 0);
	float3 dy = float3(0, e, 0);
	float3 dz = float3(0, 0, e);

	float3 p0X = snoiseVec3(p - dx);
	float3 p1X = snoiseVec3(p + dx);
	float3 p0Y = snoiseVec3(p - dy);
	float3 p1Y = snoiseVec3(p + dy);
	float3 p0Z = snoiseVec3(p - dz);
	float3 p1Z = snoiseVec3(p + dz);

	float x = p1Y.z - p0Y.z - p1Z.y + p0Z.y;
	float y = p1Z.x - p0Z.x - p1X.z + p0X.z;
	float z = p1X.y - p0X.y - p1Y.x + p0Y.x;

	const float divisor = 1 / (2 * e);
	return normalize(float3(x, y, z) * divisor);
}

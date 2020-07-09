//a‚Ìn‚É‚æ‚éè—]
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
float permute(float a)
{
	return mod((a * 34.0 + 1.0) * a, 289);
}

float4 taylorInvSqrt(float4 r)
{
	return 1.79284291400159f - r * 0.85373472095314f;
}
//rand
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


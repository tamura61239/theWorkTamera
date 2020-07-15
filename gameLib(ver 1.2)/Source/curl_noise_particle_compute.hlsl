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
	float3 vec0;
	float3 vec1;
	float2 dummy;
}
#if 1
float4 grad4(float j, float4 ip)
{
	float4 ones = float4(1, 1, 1, -1);
	float4 p, s;
	p.xyz = floor(frac(float3(j,j,j)*ip.xyz) * 7) * ip.z - 1;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	//step(a,x):(x >= a) ? 1 : 0 (xがa以上の場合は1, それ以外は0を返します)
	//lessThan(p,0):要素ごとのx<yの比較を返す
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
	//floor(x):x以下の最大の整数を返す(floorは小数点以下の切り捨てに使える)
	//①vにvとある方向との射影ベクトルを足してそれより少し小さいベクトルを取得
	float4 i = floor(v + dot(v, float4(0.309016994374947451, 0.309016994374947451, 0.309016994374947451, 0.309016994374947451)));
	//
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
	//saturate(x):x を [0, 1] の範囲にクランプします。([0, 1] の範囲に押し込む的なもの)
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
	//様々なnoiseを混ぜる
	float4 d = temp0.x * x0 + temp0.y * x1 + temp0.z * x2
		     + temp1.x * x3 + temp1.y * x4 + mmm0.x * p1 
		     + mmm0.y * p2 + mmm1.x * p3 + mmm1.y * p4;
	return normalize(d.xyz) * noiseSize * d.w;
}
float3 curl(in float3 p, in float noiseTime, in float persistence) {

	float3 xNoisePotentialDerivatives = float3(0,0,0);
	float3 yNoisePotentialDerivatives = float3(0,0,0);
	float3 zNoisePotentialDerivatives = float3(0,0,0);

	for (int i = 0; i < 3; ++i) {

		float twoPowI = pow(2.0, float(i));
		float scale = 0.5 * twoPowI * pow(persistence, float(i));

		xNoisePotentialDerivatives += lerp(snoise(float3(p * twoPowI)*noiseTime),normalize(snoise(float3(p * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
		yNoisePotentialDerivatives += lerp(snoise(float3((p + vec0) * twoPowI) * noiseTime), normalize(snoise(float3((p + vec0) * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
		zNoisePotentialDerivatives += lerp(snoise(float3((p + vec1) * twoPowI)* noiseTime), normalize(snoise(float3((p + vec1) * twoPowI) * noiseTime)), noizeNormalizeFlag) * scale;
	}

	return float3(
		zNoisePotentialDerivatives.y - yNoisePotentialDerivatives.z,
		xNoisePotentialDerivatives.z - zNoisePotentialDerivatives.x,
		yNoisePotentialDerivatives.x - xNoisePotentialDerivatives.y
	);

}

float3 curlNoise(float3 p) {
	float3 noise0 = snoise(p);
	float3 noise1 = snoise(vec0);
	float3 noise2 = snoise(vec1);
	return normalize(noise0 + noise1 + noise2);
}
#else
float3 snoise_grad(float3 v)
{
	const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);

	// First corner
	float3 i = floor(v + dot(v, C.yyy));
	float3 x0 = v - i + dot(i, C.xxx);

	// Other corners
	float3 g = step(x0.yzx, x0.xyz);
	float3 l = 1.0 - g;
	float3 i1 = min(g.xyz, l.zxy);
	float3 i2 = max(g.xyz, l.zxy);

	// x1 = x0 - i1  + 1.0 * C.xxx;
	// x2 = x0 - i2  + 2.0 * C.xxx;
	// x3 = x0 - 1.0 + 3.0 * C.xxx;
	float3 x1 = x0 - i1 + C.xxx;
	float3 x2 = x0 - i2 + C.yyy;
	float3 x3 = x0 - 0.5;

	// Permutations
	i = mod(i, 289); // Avoid truncation effects in permutation
	float4 p =
		permute(permute(permute(i.z + float4(0.0, i1.z, i2.z, 1.0))
			+ i.y + float4(0.0, i1.y, i2.y, 1.0))
			+ i.x + float4(0.0, i1.x, i2.x, 1.0));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float4 j = p - 49.0 * floor(p / 49.0);  // mod(p,7*7)

	float4 x_ = floor(j / 7.0);
	float4 y_ = floor(j - 7.0 * x_);  // mod(j,N)

	float4 x = (x_ * 2.0 + 0.5) / 7.0 - 1.0;
	float4 y = (y_ * 2.0 + 0.5) / 7.0 - 1.0;

	float4 h = 1.0 - abs(x) - abs(y);

	float4 b0 = float4(x.xy, y.xy);
	float4 b1 = float4(x.zw, y.zw);

	//float4 s0 = float4(lessThan(b0, 0.0)) * 2.0 - 1.0;
	//float4 s1 = float4(lessThan(b1, 0.0)) * 2.0 - 1.0;
	float4 s0 = floor(b0) * 2.0 + 1.0;
	float4 s1 = floor(b1) * 2.0 + 1.0;
	float4 sh = -step(h, 0.0);

	float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	float3 g0 = float3(a0.xy, h.x);
	float3 g1 = float3(a0.zw, h.y);
	float3 g2 = float3(a1.xy, h.z);
	float3 g3 = float3(a1.zw, h.w);

	// Normalise gradients
	float4 norm = taylorInvSqrt(float4(dot(g0, g0), dot(g1, g1), dot(g2, g2), dot(g3, g3)));
	g0 *= norm.x;
	g1 *= norm.y;
	g2 *= norm.z;
	g3 *= norm.w;

	// Compute noise and gradient at P
	float4 m = max(0.6 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	float4 m2 = m * m;
	float4 m3 = m2 * m;
	float4 m4 = m2 * m2;
	float3 grad =
		-6.0 * m3.x * x0 * dot(x0, g0) + m4.x * g0 +
		-6.0 * m3.y * x1 * dot(x1, g1) + m4.y * g1 +
		-6.0 * m3.z * x2 * dot(x2, g2) + m4.z * g2 +
		-6.0 * m3.w * x3 * dot(x3, g3) + m4.w * g3;
	float4 px = float4(dot(x0, g0), dot(x1, g1), dot(x2, g2), dot(x3, g3));
	return noiseSize * float3(grad/*, dot(m4, px)*/);
}
#endif
float4 SetPosition(uint dtid)
{
	float d = 1.0f / (float)(randXorshift(maxParticle));
	//float z = (float)(randXorshift(dtid)) * d-0.5f;
	//float angle = radians(z * 360)- radians(180);
	//float x = sqrt(1 - z * z) * cos(angle);
	//float y = sqrt(1 - z * z) * sin(angle);
	//float d = 1.0f / 4294967296.0f;
	float f0 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f1 = (float)(randXorshift(dtid)) * d - 0.5f;
	float f2 = (float)(randXorshift(dtid)) * d - 0.5f;
	float3 normalF3 = normalize(float3(f0, f1, f2)) * 0.2f;
	normalF3 *= (float)(randXorshift(dtid)) * d * 0.01f;
	float3 position = float3(normalF3.x + startPosition.x, normalF3.y + startPosition.y, normalF3.z + startPosition.z);
	return float4(position * 0.1f, 1);
	//return float4(x, y, z, 1);
}
float4 SetPosition(float2 uv)
{
	float x = nrand(uv);
	float y = nrand(uv);
	float z = nrand(uv);
	float3 position = normalize(float3(x, y, z));
	return float4(position, 1);
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
	velocity += lerp(snoise(position.xyz - startPosition), normalize(snoise(position.xyz - startPosition)), noizeNormalizeFlag);
	//velocity += curl(position.xyz - startPosition, speed,0.4f);
	//velocity += curl(snoise(position.xyz - startPosition), speed,0.4f);
	//velocity += lerp(curlNoise(position.xyz-startPosition),normalize(curlNoise(position.xyz - startPosition)), noizeNormalizeFlag);
	position.xyz += velocity*speed;
	//初期化
	float set = step(0, life);
	position = lerp(SetPosition(DTid.x), position, set);
	velocity = lerp(normalize(startPosition-position.xyz), velocity, set);
	life = lerp(2, life, set);
	color = lerp(endColor, startColor, max(life / 2, 0));
	//color.a += 1.f;
	rwBuffer.Store4(bufferIndex + 0, asuint(position));
	rwBuffer.Store3(bufferIndex + 16, asuint(velocity));
	rwBuffer.Store4(bufferIndex + 28, asuint(color));
	rwBuffer.Store(bufferIndex + 44, asuint(life));
}

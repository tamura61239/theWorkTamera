struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float3 worldNormal:TEXCOORD1;
	float3 worldPosition:TEXCOORD2;
};

cbuffer CbScene:register(b0)
{
	float4 lightColor;
	float4 lightDirection;
	float4 ambientColor;
	float4 eyePosition;
	row_major float4x4 view;
	row_major float4x4 projection;
};

cbuffer CbObj:register(b1)
{
	float4 materialColor;
	row_major float4x4 world;
};
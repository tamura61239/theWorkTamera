#include"gpu_cloud_particle_render.hlsli"
[maxvertexcount(4)]
void main(
	point VS_OUT input[1], 
	inout TriangleStream< GS_OUT > output
)
{
	float4 squarePosition[4] =
	{
		{ -1,1,0,0 },
		{ 1,1,0,0 },
		{ -1,-1,0,0 },
		{ 1,-1,0,0 },
	};
	float2 texcoord[4] =
	{
		{ 0,0 },
		{ 1,0 },
		{ 0,1 },
		{ 1,1 },
	};
	for (int i = 0;i < 4;i++)
	{
		GS_OUT gout;
		gout.position = mul(input[0].position+ squarePosition[i], projection);
		gout.texcoord = texcoord[i];
		gout.color = input[0].color;
		output.Append(gout);
	}
	output.RestartStrip();
}

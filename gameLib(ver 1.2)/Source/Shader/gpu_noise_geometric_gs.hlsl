#include"gpu_noise_geometric.hlsli"

[maxvertexcount(30)]
void main(
	point VS_OUT input[1],
	inout LineStream< GS_OUT > output
)
{
	//for (uint i = 0; i < 3; i++)
	//{
	//	GSOutput element;
	//	element.pos = input[i];
	//	output.Append(element);
	//}
	float3 lineBox[8] =
	{
		{0.5,0.5,0.5},
		{-0.5,0.5,0.5},
		{-0.5,-0.5,0.5},
		{0.5,-0.5,0.5},
		{0.5,0.5,-0.5},
		{-0.5,0.5,-0.5},
		{-0.5,-0.5,-0.5},
		{0.5,-0.5,-0.5},
	};
	float3x3 angleMatrix = mul(MakeRotation(float3(1, 0, 0), input[0].angle.x),mul(MakeRotation(float3(0, 1, 0), input[0].angle.y),MakeRotation(float3(0, 0, 1), input[0].angle.z)));
	float3 position = (float3)0;
	float4 viewPosition = (float4)0;
	for (int i = 0;i < 2;i++)
	{
		GS_OUT gout;
		//line0
	    position = input[0].position.xyz + mul(lineBox[0+i*4], angleMatrix);
	    viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		position = input[0].position.xyz + mul(lineBox[1 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//line1
		position = input[0].position.xyz + mul(lineBox[1 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		position = input[0].position.xyz + mul(lineBox[2 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//line2
		position = input[0].position.xyz + mul(lineBox[2 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		position = input[0].position.xyz + mul(lineBox[3 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//line3
		position = input[0].position.xyz + mul(lineBox[3 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		position = input[0].position.xyz + mul(lineBox[0 + i * 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
	}
	for (int i = 0;i < 4;i++)
	{
		GS_OUT gout;
		position = input[0].position.xyz + mul(lineBox[i], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		position = input[0].position.xyz + mul(lineBox[i + 4], angleMatrix);
		viewPosition = mul(float4(position, 1), view);
		gout.position = mul(viewPosition, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
	}
}
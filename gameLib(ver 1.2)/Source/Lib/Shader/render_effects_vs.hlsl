#include"render_effects.hlsli"
/**********************************************************************/
//�@�@�@�X�N���[���S�̂ɕ`��ł���悤�ȃe�N�X�`���̒��_��UV�����߂�
/**********************************************************************/

VS_OUT main(uint vertex_id : SV_VERTEXID)
{
	VS_OUT vout;
	vout.texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);
	vout.position = float4(vout.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	//vout.position = float4(vout.texcoord * float2(1.0f, -1.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	return vout;
}
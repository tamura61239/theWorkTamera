#include"static_mesh.hlsli"
/*****************************************************/
//�@�@�@�@	�A���t�@�[�l�����ȏ�̂̐F�����擾
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
    if (pin.color.a <= 0.3f)
    {
        discard;
    }
    return pin.color;
}
#include "sprite.hlsli"
Texture2D diffuse_map : register(t0);
Texture2D velocity_map : register(t1);
SamplerState diffuse_map_sampler_state : register(s0);
/****************************************************************************/
//�@�@�@���x�}�b�v���g���ău���[��������
/****************************************************************************/

float2 rand2(float2 s)
{
    s = float2(dot(s, float2(12.9898f, 78.233f)), dot(s, float2(269.5f, 183.3f)));
    return frac(sin(s) * float2(43758.5453123f, 43758.5453123f));
}

float4 main(VS_OUT pin) : SV_TARGET
{
   //	�u���[�����̎Z�o
    const float4 velocityMap = velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    const float2 velocity = velocityMap.xy;

	//	�u���[������ׂ����߂̌W��
    float2 jitter = 1.0f - (rand2(velocityMap.xy) - 0.5f);

	//	���x�x�N�g�������ɂ��ău���[��������
    float4 color = float4(0,0,0,0);
    int loop = 8 / 2;
    
    for (int ii = -loop; ii <= loop; ++ii)
    {
        float2 uv = pin.texcoord + ii * ((velocity * jitter) / 8);
        color += diffuse_map.Sample(diffuse_map_sampler_state, uv);
    }
    color.xyz /= color.a;

    return color;
}
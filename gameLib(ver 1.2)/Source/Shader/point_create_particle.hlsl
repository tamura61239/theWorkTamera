#include"particle_data.hlsli"
#include"Lib/rand_function.hlsli"

cbuffer CreateData : register(b1)
{
    float3 position;
    float speed;
    float4 color;
    float life;
    float3 scale;
    float accel;
};

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //死んでる分のカウントを－1する
    uint deleteCount = 0;
    countBuffer.InterlockedAdd(8, -1, deleteCount);
    if (deleteCount > 0)
    {
        //死んでるパーティクルのindexを取得   
        uint newParticleIndex = deathIndexBuffer[deleteCount - 1];
        //パーティクルのデータ作る
        ParticleData particle = (ParticleData) 0;
        particle.position = position;
        particle.velocity = normalize(float3(rand_2_normal(float2(deleteCount % 675, newParticleIndex % 234), 1), rand_1_normal(float2(newParticleIndex % 498, deleteCount % 321), 1))) * speed;
        particle.accel = normalize(float3(rand_1_normal(float2(deleteCount % 159, newParticleIndex % 621), 1), rand_2_normal(float2(newParticleIndex % 765, deleteCount % 437), 1))) * accel;
        particle.color = color;
        particle.life = life;
        //パーティクルのデータを設定
        particleBuffer[newParticleIndex] = particle;
        
        //パーティクルのカウントを増やす
        uint particleCount = 0;
        countBuffer.InterlockedAdd(0, 1, particleCount);
        indexBuffer.Store(particleCount * 4, newParticleIndex);
    }
}
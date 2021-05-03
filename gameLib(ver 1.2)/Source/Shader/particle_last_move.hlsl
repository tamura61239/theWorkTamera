#include"particle_data.hlsli"


[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleCount = countBuffer.Load(0);
    if (particleCount > DTid.x)
    {
        uint particleIndex = indexBuffer.Load(DTid.x * 4);
        ParticleData particle = particleBuffer[particleIndex];
        if (particle.life < particle.timer)
        {
            //動きの更新
            particle.position += particle.velocity * elapsdTime;
            particle.timer += elapsdTime;
            particleBuffer[particleIndex] = particle;
            //描画用データの初期化
            RenderParticle render = (RenderParticle) 0;
            render.position = particle.position;
            render.angle = particle.angle;
            render.scale = particle.scale;
            render.velocity = particle.velocity;
            render.color |= ((uint) (particle.color.r * 255) & 0x00FFFFFF) << 24;
            render.color |= ((uint) (particle.color.g * 255) & 0x00FFFFFF) << 16;
            render.color |= ((uint) (particle.color.b * 255) & 0x00FFFFFF) << 8;
            render.color |= ((uint) (particle.color.a * 255) & 0x00FFFFFF) << 0;
            WriteRender(render, maxRenderDataSize * particleIndex);

            //生きているカウントを増やす
            uint newAliveCount;
            countBuffer.InterlockedAdd(4, 1, newAliveCount);
            
            newIndexBuffer.Store(newAliveCount * 4, particleIndex);
        }
        else
        {
            //死んでる分のカウントを増やす
            uint deleteCount;
            countBuffer.InterlockedAdd(8, 1, deleteCount);
            deathIndexBuffer[deleteCount] = particleIndex;

        }
    }
}
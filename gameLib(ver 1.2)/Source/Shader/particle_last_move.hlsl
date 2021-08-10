#include"particle_data.hlsli"
#include"color_conversion.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleCount = countBuffer.Load(0);
    if (particleCount > DTid.x)
    {
        uint particleIndex = indexBuffer.Load(DTid.x * 4);
        ParticleData particle = particleBuffer[particleIndex];
        if (particle.life > 0)
        {
            //�����̍X�V
            particle.position += particle.velocity * elapsdTime;
            particle.life -= elapsdTime;
            particleBuffer[particleIndex] = particle;
            //�`��p�f�[�^�̏�����
            RenderParticle render = (RenderParticle) 0;
            render.position = float4(particle.position, 1.0f);
            render.angle = particle.angle;
            render.scale = particle.scale;
            render.velocity = particle.velocity;
            render.color = particle.color;
            WriteRender(render, maxRenderDataSize * particleIndex);

            //�����Ă���J�E���g�𑝂₷
            uint newAliveCount;
            countBuffer.InterlockedAdd(4, 1, newAliveCount);
            
            newIndexBuffer.Store(newAliveCount * 4, particleIndex);
        }
        else
        {
            //����ł镪�̃J�E���g�𑝂₷
            uint deleteCount;
            countBuffer.InterlockedAdd(8, 1, deleteCount);
            deathIndexBuffer[deleteCount] = particleIndex;

        }
    }
}
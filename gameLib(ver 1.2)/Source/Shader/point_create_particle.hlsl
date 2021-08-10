#include"particle_data.hlsli"
#include"../Lib/Shader/rand_function.hlsli"


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //����ł镪�̃J�E���g���|1����
    uint deleteCount = 0;
    countBuffer.InterlockedAdd(8, -1, deleteCount);
   // if (deleteCount > 0)
    {
        //����ł�p�[�e�B�N����index���擾   
        uint newParticleIndex = deathIndexBuffer[deleteCount - 1];
        //�p�[�e�B�N���̃f�[�^���
        ParticleData particle = (ParticleData) 0;
        particle.position = position;
        particle.velocity = normalize(float3(rand_1_normal(float2(deleteCount % 675, newParticleIndex % 234), 1) - 1, rand_1_normal(float2(deleteCount % 234, newParticleIndex % 543), 1) - 1, rand_1_normal(float2(newParticleIndex % 498, deleteCount % 321), 1) - 1)) * speed;
        particle.accel = normalize(float3(rand_1_normal(float2(deleteCount % 159, newParticleIndex % 621), 1) - 1, rand_1_normal(float2(deleteCount % 159, newParticleIndex % 621), 1) - 1, rand_1_normal(float2(deleteCount % 159, newParticleIndex % 621), 1) - 1)) * accel;
        particle.color = color;
        particle.life = life;
        particle.scale = scale;
        //�p�[�e�B�N���̃f�[�^��ݒ�
        particleBuffer[newParticleIndex] = particle;
        
        //�p�[�e�B�N���̃J�E���g�𑝂₷
        uint particleCount = 0;
        countBuffer.InterlockedAdd(0, 1, particleCount);
        indexBuffer.Store(particleCount * 4, newParticleIndex);
    }
}
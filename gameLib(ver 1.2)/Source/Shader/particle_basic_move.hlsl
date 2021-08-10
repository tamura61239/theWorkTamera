#include"particle_data.hlsli"

cbuffer MoveData : register(b2)
{
    float4 endColor;
    float maxSpeed;
    float colorChangeTime;
    float2 dummy;
}

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    //uint particleCount = indexBuffer.Load(0);
    //if (particleCount > DTid.x)
    //{
    //    uint particleIndex = indexBuffer.Load(DTid.x * 4);
    //    ParticleData particle = particleBuffer[particleIndex];
        
    //    particle.velocity += particle.accel * elapsdTime;
        
    //    float len = length(particle.velocity);
        
    //    float3 vec = normalize(particle.velocity);
        
    //    if(len>maxSpeed)
    //    {
    //        particle.velocity = vec * maxSpeed;
    //    }
    //    particle.color = lerp(particle.color, endColor, min(colorChangeTime, particle.timer));
        
    //    particleBuffer[particleIndex] = particle;
    //}
}
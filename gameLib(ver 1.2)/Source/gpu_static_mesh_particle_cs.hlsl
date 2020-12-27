#include"gpu_static_mesh_particle.hlsli"
#include"curl_noise.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle p = particleBuffer[DTid.x];
    float speed = length(p.velocity);
    p.velocity = normalize(snoise(normalize(p.velocity))) * speed;
    p.position += p.velocity * elapsdTime;
    p.color.a = p.life;
    p.life -= elapsdTime;
    float positionW = 1;
    if (p.life <= 0)
    {
        p.color *= 0.f;
        p.position *= 0.f;
        p.life *= 0.f;
        p.scale *= 0.f;
        p.velocity *= 0.f;
        positionW = 0;

    }
    particleBuffer[DTid.x] = p;
    RenderParticle render = (RenderParticle) 0;
    render.position = float4(p.position, positionW);
    render.scale = p.scale;
    render.color = p.color;
    render.velocity = p.velocity;
    WriteRenderBuffer(DTid.x * 17*4, render);
}
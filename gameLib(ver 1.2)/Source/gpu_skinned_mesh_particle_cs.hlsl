#include"gpu_skinned_mesh_particle.hlsli"
#include"curl_noise.hlsli"


[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Particle p = particleBuffer[DTid.x];
    float speed = length(p.velocity);
    p.velocity = normalize(snoise(normalize(p.velocity))) * speed;

    p.position += p.velocity * elapsdTime;
    p.life -= elapsdTime;
    
    float4 color = (float4) 0;
    color.r = ((p.color >> 24) & 0x000000FF) / 255.0f;
    color.g = ((p.color >> 16) & 0x000000FF) / 255.0f;
    color.b = ((p.color >> 8) & 0x000000FF) / 255.0f;
    color.a = ((p.color >> 0) & 0x000000FF) / 255.0f;
    color.a=p.life;
    float positionW = 1;
    if (p.life <= 0)
    {
        p.position *= 0.f;
        p.life *= 0.f;
        p.scale *= 0.f;
        p.velocity *= 0.f;
        positionW *= 0.f;
        color *= 0.f;

    }
    p.color |= ((uint)(color.r * 255) & 0x000000FF) << 24;
    p.color |= ((uint)(color.g * 255) & 0x000000FF) << 16;
    p.color |= ((uint)(color.g * 255) & 0x000000FF) << 8;
    p.color |= ((uint)(color.a * 255) & 0x000000FF) << 0;

    particleBuffer[DTid.x] = p;
    
    RenderParticle render = (RenderParticle) 0;
    render.position = float4(p.position, positionW);
    render.color = p.color;
    render.velocity = p.velocity;
    render.color = p.color;
    WriteRenderBuffer(DTid.x * 4 * 10, render);
}
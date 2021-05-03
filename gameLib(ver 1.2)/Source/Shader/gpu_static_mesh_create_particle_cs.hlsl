#include"gpu_static_mesh_particle.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    int i = 0;
    float3 position = (float3) 0;
    float3 normal = (float3) 0;
    for (i = 0; i < 3;i++)
    {
        position += vertexBuffer[indexBuffer[DTid.x*3+1]].position;
        normal += vertexBuffer[indexBuffer[DTid.x * 3 + 1]].normal;

    }
    position /= 3;
    normal /= 3;
        Particle p = (Particle) 0;
    p.position = mul(float4(position, 1), world).xyz;
    p.scale = 0.1f;
    p.color = float4(color, 1);
    p.life = 1;
    p.velocity = normal*10.f;
    particleBuffer[DTid.x+startIndex] = p;

}
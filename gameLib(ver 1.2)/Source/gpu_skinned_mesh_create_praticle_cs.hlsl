#include"gpu_skinned_mesh_particle.hlsli"
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 position = { 0, 0, 0 }, normal = { 0, 0, 0 };
    int i = 0, j = 0;
    for (i = 0; i < 3; i++)
    {
        uint index = DTid.x*3+i;
        Vertex vertex = vertexBuffer[index];
        for (j = 0; j < 4;j++)
        {
            position += (vertex.bone_weight[j] * mul(float4(vertex.position, 1), boneWorld[vertex.bone_index[j]])).xyz;
            normal += (vertex.bone_weight[j] * mul(vertex.normal, (float3x3) boneWorld[vertex.bone_index[j]])).xyz;
        }
    }
    position /= 3;
    normal = normalize(normal);
    Particle p = (Particle) 0;
    p.position = position;
    p.color = startColor;
    p.life = 0.2f;
    p.scale = 0.1f;
    p.velocity = normal * 5.f;
    particleBuffer[startIndex + DTid.x] = p;
}
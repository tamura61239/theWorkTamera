#include"gpu_skinned_mesh_particle.hlsli"
#include"Lib/rand_function.hlsli"
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 position = { 0, 0, 0 }, normal = { 0, 0, 0 };
    int i = 0, j = 0;
    //for (i = 0; i < 3; i++)
    //{
    //    uint index = DTid.x * 3 + i;
    //    Vertex vertex = vertexBuffer[index];
    //    for (j = 0; j < 4; j++)
    //    {
    //        position += (vertex.bone_weight[j] * mul(float4(vertex.position, 1), boneWorld[vertex.bone_index[j]])).xyz;
    //        normal += (vertex.bone_weight[j] * mul(vertex.normal, (float3x3) boneWorld[vertex.bone_index[j]])).xyz;
    //    }
    //}
    //position /= 3;
    float3 pos[3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
    for (i = 0; i < 3; i++)
    {
        uint index = indexBuffer[DTid.x * 3 + i];
        Vertex vertex = vertexBuffer[index];
        for (j = 0; j < 4; j++)
        {
            pos[i] += (vertex.bone_weight[j] * mul(float4(vertex.position, 1), boneWorld[vertex.bone_index[j]])).xyz;
            normal += (vertex.bone_weight[j] * mul(vertex.normal, (float3x3) boneWorld[vertex.bone_index[j]])).xyz;
        }
    }
    
    float3 vec1 = (pos[1] - pos[0]) * saturate(rand_1_normal(float2((startIndex + DTid.x) % 621, DTid.x * 3 % 439), 0.5f));
    float3 vec2 = (pos[2] - pos[0]) * saturate(rand_1_normal(float2(DTid.x * 3 % 756, (startIndex + DTid.x) % 394), 0.5f));
    
    float3 vec3 = vec2 - vec1;
    
    vec3 *= saturate(rand_1_normal(float2((startIndex + DTid.x) % 567, (startIndex + DTid.x) % 381), 0.5f));
    position = pos[0] + vec1 + vec3;
    normal = normalize(normal);
    Particle p = (Particle) 0;
    p.position = position;
    p.color = startColor;
    p.life = life;
    p.scale = scale;
    p.velocity = normal * speed;
    particleBuffer[startIndex + DTid.x] = p;
}
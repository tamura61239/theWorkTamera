struct Particle
{
    float3 position;
    float3 velocity;
    float scale;
    float life;
    uint color;
};
struct RenderParticle
{
    float4 position;
    float angle;
    uint color;
    float3 velocity;
    float scale;
};
cbuffer CbCreate : register(b0)
{
    float startIndex;
    uint startColor;
    float scale;
    float life;
    float speed;
};
cbuffer CbBone : register(b1)
{
    row_major float4x4 boneWorld[128];
};
cbuffer CbUpdate: register(b2)
{
    float elapsdTime;
};
struct Vertex
{
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 bone_weight;
    uint4 bone_index;
};
RWStructuredBuffer<Particle> particleBuffer : register(u0);

StructuredBuffer<Vertex> vertexBuffer : register(t0);
StructuredBuffer<int> indexBuffer : register(t1);

RWByteAddressBuffer renderBuffer : register(u1);
void WriteRenderBuffer(uint bufferIndex, RenderParticle p)
{
    renderBuffer.Store4(bufferIndex, asuint(p.position));
    bufferIndex += 4 * 4;
    renderBuffer.Store(bufferIndex, asuint(p.angle));
    bufferIndex += 4 * 1;
    renderBuffer.Store(bufferIndex, p.color);
    bufferIndex += 4 * 1;
    renderBuffer.Store3(bufferIndex, asuint(p.velocity));
    bufferIndex += 4 * 3;
    renderBuffer.Store(bufferIndex, asuint(p.scale));

}

struct Vertex
{
    float3 position;
    float3 normal;
    float2 texcoord;
};
struct Particle
{
    float3 position;
    float3 velocity;
    float4 color;
    float life;
    float scale;
};
struct RenderParticle
{
    float4 position;
    float3 angle;
    float4 color;
    float3 velocity;
    float3 scale;
};
cbuffer CbCreate : register(b0)
{
    float3 color;
    int startIndex;
    row_major float4x4 world;

}
cbuffer CbUpdate : register(b1)
{
    float elapsdTime;
    float3 dummy;
}
RWByteAddressBuffer renderBuffer : register(u1);
void WriteRenderBuffer(uint bufferIndex,RenderParticle p)
{
    renderBuffer.Store4(bufferIndex, asuint(p.position));
    bufferIndex += 4 * 4;
    renderBuffer.Store3(bufferIndex, asuint(p.angle));
    bufferIndex += 4 * 3;
    renderBuffer.Store4(bufferIndex, asuint(p.color));
    bufferIndex += 4 * 4;
    renderBuffer.Store3(bufferIndex, asuint(p.velocity));
    bufferIndex += 4 * 3;
    renderBuffer.Store3(bufferIndex, asuint(p.scale));

}
RWStructuredBuffer<Particle> particleBuffer : register(u0);

StructuredBuffer<Vertex> vertexBuffer : register(t0);
StructuredBuffer<uint> indexBuffer : register(t1);

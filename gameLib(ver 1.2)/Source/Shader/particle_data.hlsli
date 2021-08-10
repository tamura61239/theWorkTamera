struct ParticleData
{
    float3 position;
    float3 scale;
    float3 angle;
    float3 velocity;
    float4 color;
   float3 accel;
    float life;
};
struct RenderParticle
{
    float4 position;
    float3 angle;
    float4 color;
    float3 velocity;
    float3 scale;
};
cbuffer CbScene : register(b0)
{
    float elapsdTime;
    float3 d2;
}
cbuffer CreateData : register(b1)
{
    float3 position;
    float speed;
    float4 color;
    float life;
    float3 scale;
    float accel;
    float3 d;
};

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
RWByteAddressBuffer countBuffer : register(u1);
RWByteAddressBuffer renderBuffer : register(u2);
RWByteAddressBuffer indexBuffer : register(u3);
RWByteAddressBuffer newIndexBuffer : register(u4);
RWStructuredBuffer<uint> deathIndexBuffer : register(u5);
static const uint maxRenderDataSize = 17 * 4;


void WriteRender(in RenderParticle render,in uint index)
{
    uint bufferIndex = index;
    renderBuffer.Store4(bufferIndex, asuint(render.position));
    bufferIndex += 16;
    renderBuffer.Store3(bufferIndex, asuint(render.angle));
    bufferIndex += 12;
    renderBuffer.Store4(bufferIndex, asuint(render.color));
    bufferIndex += 16;
    renderBuffer.Store3(bufferIndex, asuint(render.velocity));
    bufferIndex += 12;
    renderBuffer.Store3(bufferIndex, asuint(render.scale));
}
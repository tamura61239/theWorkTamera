struct ParticleData
{
    float3 position;
    float3 scale;
    float3 angle;
    float3 velocity;
    float4 color;
   float3 accel;
    float life;
    float timer;
};
struct RenderParticle
{
    float3 position;
    float3 scale;
    float3 angle;
    float3 velocity;
    uint color;
};
RWStructuredBuffer<ParticleData> particleBuffer : register(u0);
RWByteAddressBuffer indexBuffer : register(u1);
RWByteAddressBuffer newIndexBuffer : register(u2);
RWStructuredBuffer<uint> deathIndexBuffer : register(u3);
RWByteAddressBuffer renderBuffer : register(u4);
RWByteAddressBuffer countBuffer : register(u5);
static const uint maxRenderDataSize = 13 * 4;
cbuffer CbScene : register(b0)
{
    float elapsdTime;
}

void WriteRender(in RenderParticle render,in uint index)
{
    renderBuffer.Store3(index, asuint(render.position));
    index += 12;
    renderBuffer.Store3(index, asuint(render.scale));
    index += 12;
    renderBuffer.Store3(index, asuint(render.angle));
    index += 12;
    renderBuffer.Store3(index, asuint(render.velocity));
    index += 12;
    renderBuffer.Store(index, render.color);
}
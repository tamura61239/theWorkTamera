struct ParticleMoveParameter
{
    float life;
    float3 accel;
    float3 anglerVelocity;
    float3 startPosition;
    float maxLife;

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

RWStructuredBuffer<ParticleMoveParameter> particleBuffer : register(u0);
RWByteAddressBuffer countBuffer : register(u1);
RWByteAddressBuffer indexBuffer : register(u3);
RWByteAddressBuffer newIndexBuffer : register(u4);
RWStructuredBuffer<uint> deathIndexBuffer : register(u5);
#include"particle_data.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint newAliveCount = countBuffer.Load(4);
    
    countBuffer.Store(0, newAliveCount);
    countBuffer.Store(4, 0);

}
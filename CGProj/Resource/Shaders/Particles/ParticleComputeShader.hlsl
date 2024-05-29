#ifndef COMPUTE_PARTICLE
#define COMPUTE_PARTICLE

#include "GlobalsParticles.hlsl"

RWStructuredBuffer<ParticleB> Particles : register(u0);

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 UV : TEXTURE0;
};

#define THREAD_GROUP_X 32
#define THREAD_GROUP_Y 24
#define THREAD_GROUP_TOTAL 768

[numthreads(THREAD_GROUP_X, THREAD_GROUP_Y, 1)]
void CS(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * 4221* THREAD_GROUP_TOTAL + groupIndex;
    [flatten]
    if (index >= MaxParticles)
        return;

    ParticleB particle = Particles[index];

    float3 position = particle.pos;
    //float3 velocity = particle.Velocity;
    float3 velocity = float3((groupID.x % 2 == 0 ? -1 : 1)*0.01f * sin(TotalTime * 4),
        -0.1f,
        (groupID.y % 2 == 0 ? -1 : 1) * 0.1f * sin(TotalTime * 3));

    // payload

    particle.pos = position + velocity * DeltaTime;
    particle.vel = velocity;

    Particles[index] = particle;

}

AppendStructuredBuffer<uint>	DeadListToAddTo		: register( u0 );

[numthreads(256,1,1)]
void CS_InitDeadList( uint3 id : SV_DispatchThreadID )
{
    DeadListToAddTo.Append( id.x );
}

RWBuffer<uint> DispatchArgs : register( u0 );

cbuffer NumElementsCB : register( b0 )
{
    int4 NumElements;
};


[numthreads(1, 1, 1)]
void InitDispatchArgs( uint3 id : SV_DispatchThreadID )
{
    DispatchArgs[ 0 ] = ((NumElements-1) >> 9) + 1;
    DispatchArgs[ 1 ] = 1;
    DispatchArgs[ 2 ] = 1;
    DispatchArgs[ 3 ] = 0;
}

#endif

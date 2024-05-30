#ifndef EMIT_PARTICLE
#define EMIT_PARTICLE

#include "GlobalsParticles.hlsl"

Texture2D RandomBuffer : register( t0 );

RWStructuredBuffer<ParticleA> ParticlesBufferA : register( u0 );
RWStructuredBuffer<ParticleB> ParticlesBufferB : register( u1 );

ConsumeStructuredBuffer<uint> DeadListToAllocFrom : register( u2 );

cbuffer EmitterConstantBuffer : register( b1 )
{
float4 EmitterViewPosition;
float4 EmitterViewVelocity;
float4 PositionVariance;

int MaxParticlesThisFrame;
float ParticleLifeSpan;
float StartSize;
float EndSize;

float VelocityVariance;
float Mass;
uint EmitterIndex;
uint EmitterStreaks;

uint TextureIndex;
};

[numthreads(1024,1,1)]
void CS_Emit(uint3 id : SV_DispatchThreadID)
{
    uint count;
    uint stride;
    DeadListToAllocFrom.GetDimensions(count, stride);
    count = NumDeadParticles;

    // Check to make sure we don't emit more particles than we specified
    if (id.x < count && id.x < MaxParticlesThisFrame)
    {
        // Initialize the particle data to zero to avoid any unexpected results
        ParticleA pa = (ParticleA)0;
        ParticleB pb = (ParticleB)0;

        // Generate some random numbers from reading the random texture
        float2 uv = float2(id.x / 1024.0, TotalTime);
        float3 randomValues0 = RandomBuffer.SampleLevel(samWrapLinear, uv, 0).xyz;

        float2 uv2 = float2((id.x + 1) / 1024.0, TotalTime);
        float3 randomValues1 = RandomBuffer.SampleLevel(samWrapLinear, uv2, 0).xyz;

        float velocityMagnitude = length(EmitterViewVelocity.xyz);

        float4 emitPos = mul(float4(EmitterViewPosition.xyz, 1.0), mWorld);
        pb.pos = emitPos.xyz + (randomValues0.xyz * PositionVariance.xyz);

        pa.emitterProperties = WriteEmitterProperties(EmitterIndex, TextureIndex, EmitterStreaks ? true : false);
        pa.rot = 0;
        pa.isSleeping = 0;
        pa.collisionCount = 0;

        pb.mass = Mass;
        pb.vel = EmitterViewVelocity.xyz + (randomValues1.xyz * velocityMagnitude * VelocityVariance);

        pb.lifespan = ParticleLifeSpan;
        pb.age = pb.lifespan == -1 ? 1.0f : pb.lifespan;
        pb.startSize = StartSize;
        pb.endSize = EndSize;

        // The index into the global particle list obtained from the dead list. 
        // Calling consume will decrement the counter in this buffer.
        uint index = DeadListToAllocFrom.Consume();

        ParticlesBufferA[index] = pa;
        ParticlesBufferB[index] = pb;
    }
}

#endif

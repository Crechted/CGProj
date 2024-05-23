#ifndef PARTICLE
#define PARTICLE

struct Particle
{
    float3 Position;
    float3 Velocity;
};

RWStructuredBuffer<Particle> Particles : register(u0);

cbuffer Handler : register(b0)
{
int GroupDim;
uint MaxParticles;
float TotalTime;
float DeltaTime;
};

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
    uint index = groupID.x * THREAD_GROUP_TOTAL + groupID.y * GroupDim * THREAD_GROUP_TOTAL + groupIndex;
    [flatten]
    if (index >= MaxParticles)
        return;

    Particle particle = Particles[index];

    float3 position = particle.Position;
    //float3 velocity = particle.Velocity;
    float3 velocity = float3((groupID.x % 2 == 0 ? -1 : 1)*0.01f * sin(TotalTime * 4),
        -0.1f,
        (groupID.y % 2 == 0 ? -1 : 1) * 0.1f * sin(TotalTime * 3));

    // payload

    particle.Position = position + velocity * DeltaTime;
    particle.Velocity = velocity;

    Particles[index] = particle;

}


#endif

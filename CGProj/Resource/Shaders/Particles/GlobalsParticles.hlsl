#ifndef GLOBALS_PARTICLES
#define GLOBALS_PARTICLES

struct ParticleA
{
    float4 tintAndAlpha; // The color and opacity

    float2 velXY;       // View space velocity XY used for streak extrusion
    float emitterNdotL; // The lighting term for the while emitter
    uint emitterProperties;
    // The index of the emitter in 0-15 bits, 16-23 for atlas index, 24th bit is whether or not the emitter supports velocity-based streaks

    float rot;           // The rotation angle
    uint isSleeping;     // Whether or not the particle is sleeping (ie, don't update position)
    uint collisionCount; // Keep track of how many times the particle has collided
    float pads[1];
};

struct ParticleB
{
    float3 pos;
    float mass;

    float3 vel;
    float lifespan;

    float distanceToEye;
    float age;
    float startSize;
    float endSize;
};

cbuffer Handler : register(b0)
{
matrix mInvProjView;
matrix mInvProj;
matrix mInvView;
matrix mView;
matrix mProj;
matrix mViewProj;

float3 CamPos;
uint MaxParticles;

float2 ScreenDimensions;
float TotalTime;
float DeltaTime;

float CollisionThickness;
int CollideParticles;
int ShowSleepingParticles;
int EnableSleepState;
};

// The number of dead particles in the system
cbuffer DeadListCount : register( b2 )
{
uint NumDeadParticles;
};


// The number of alive particles this frame
cbuffer ActiveListCount : register( b3 )
{
uint NumActiveParticles;
};

uint GetEmitterIndex(uint emitterProperties)
{
    return emitterProperties & 0xffff;
}


float GetTextureOffset(uint emitterProperties)
{
    uint index = (emitterProperties & 0x000f0000) >> 16;

    return (float)index / 2.0f;
}


bool IsStreakEmitter(uint emitterProperties)
{
    return (emitterProperties >> 24) & 0x01 ? true : false;
}

uint WriteEmitterProperties(uint emitterIndex, uint textureIndex, bool isStreakEmitter)
{
    uint properties = emitterIndex & 0xffff;

    properties |= textureIndex << 16;

    if (isStreakEmitter)
    {
        properties |= 1 << 24;
    }

    return properties;
}

float2 calcEllipsoidRadius( float radius, float2 viewSpaceVelocity )
{
    float minRadius = radius * max( 1.0, 0.1*length( viewSpaceVelocity ) );
    return float2( radius, minRadius );
}

float3 ConvertProjToView(float4 p, matrix mProjectionInv)
{
    p = mul(p, mProjectionInv);
    p /= p.w;

    return p.xyz;
}

SamplerState samWrapLinear : register( s0 );
SamplerState samClampLinear : register( s1 );

#endif

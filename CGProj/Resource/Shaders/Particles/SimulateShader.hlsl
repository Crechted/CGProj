#ifndef SIMULATE_PARTICLE
#define SIMULATE_PARTICLE

#include "GlobalsParticles.hlsl"
#define USE_GEOMETRY_SHADER

// Particle buffer in two parts
RWStructuredBuffer<ParticleA> ParticleBufferA : register( u0 );
RWStructuredBuffer<ParticleB> ParticleBufferB : register( u1 );

// The dead list, so any particles that are retired this frame can be added to this list
AppendStructuredBuffer<uint> DeadListToAddTo : register( u2 );

// The alive list which gets built using this shader
RWStructuredBuffer<float2> AliveIndexBuffer : register( u3 );

// Viewspace particle positions are calculated here and stored
RWStructuredBuffer<float4> ViewSpacePositions : register( u4 );

// The maximum radius in XY is calculated here and stored
RWStructuredBuffer<float> MaxRadiusBuffer : register( u5 );

// The draw args for the DrawInstancedIndirect call needs to be filled in before the rasterization path is called, so do it here
RWBuffer<uint> DrawArgs : register( u6 );

// The opaque scene's depth buffer read as a texture
Texture2D DepthBuffer : register( t0 );


// Calculate the view space position given a point in screen space and a texel offset
float3 calcViewSpacePositionFromDepth(float2 normalizedScreenPosition, int2 texelOffset)
{
    float2 uv;

    // Add the texel offset to the normalized screen position
    normalizedScreenPosition.x += (float)texelOffset.x / ScreenDimensions.x;
    normalizedScreenPosition.y += (float)texelOffset.y / ScreenDimensions.y;

    // Scale, bias and convert to texel range
    uv.x = (0.5 + normalizedScreenPosition.x * 0.5) * ScreenDimensions.x;
    uv.y = (1 - (0.5 + normalizedScreenPosition.y * 0.5)) * ScreenDimensions.y;

    // Fetch the depth value at this point
    float depth = DepthBuffer.Load(uint3(uv.x, uv.y, 0)).x;

    // Generate a point in screen space with this depth
    float4 viewSpacePosOfDepthBuffer;
    viewSpacePosOfDepthBuffer.xy = normalizedScreenPosition.xy;
    viewSpacePosOfDepthBuffer.z = depth;
    viewSpacePosOfDepthBuffer.w = 1;

    // Transform into view space using the inverse projection matrix
    viewSpacePosOfDepthBuffer = mul(viewSpacePosOfDepthBuffer, mInvProj);
    viewSpacePosOfDepthBuffer.xyz /= viewSpacePosOfDepthBuffer.w;

    return viewSpacePosOfDepthBuffer.xyz;
}


// Simulate 256 particles per thread group, one thread per particle
[numthreads(256,1,1)]
void CS_Simulate(uint3 id : SV_DispatchThreadID)
{
    // Initialize the draw args using the first thread in the Dispatch call
    if (id.x == 0)
    {
        DrawArgs[0] = 0; // Number of primitives reset to zero
        DrawArgs[1] = 1; // Number of instances is always 1
        DrawArgs[2] = 0;
        DrawArgs[3] = 0;
        DrawArgs[4] = 0;
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    const float3 vGravity = float3(0.0, -9.81, 0.0);

    // Fetch the particle from the global buffer
    ParticleA pa = ParticleBufferA[id.x];
    ParticleB pb = ParticleBufferB[id.x];

    // If the partile is alive
    if (pb.lifespan == -1) pb.age = 1.0f;
    if (pb.age > 0.0f)
    {
        // Extract the individual emitter properties from the particle
        //uint emitterIndex = GetEmitterIndex( pa.emitterProperties );
        bool streaks = IsStreakEmitter(pa.emitterProperties);

        // Age the particle by counting down from Lifespan to zero
        if (pb.lifespan != -1) pb.age -= DeltaTime;

        // Update the rotation
        if (pb.lifespan != -1) pa.rot += 0.24 * DeltaTime;

        float3 vNewPosition = pb.pos;

        // Apply force due to gravity
        if (pa.isSleeping == 0)
        {
            pb.vel += pb.mass * vGravity * DeltaTime;

            // Apply a little bit of a wind force
            float3 windDir = float3(0.1, 1, 0);
            float windStrength = 0.0;

            pb.vel += normalize(windDir) * windStrength * DeltaTime;

            // Calculate the new position of the particle
            vNewPosition += pb.vel * DeltaTime;
        }

        // Calculate the normalized age
        float fScaledLife = pb.lifespan != -1
                                ? 1.0 - saturate(pb.age / pb.lifespan)
                                : 0.0f;

        // Calculate the size of the particle based on age
        float radius = lerp(pb.startSize, pb.endSize, fScaledLife);

        // By default, we are not going to kill the particle
        bool killParticle = false;

        if (CollideParticles)
        {
            // Transform new position into view space
            float3 viewSpaceParticlePosition = mul(float4(vNewPosition, 1), mView).xyz;

            // Also obtain screen space position
            float4 screenSpaceParticlePosition = mul(float4(vNewPosition, 1), mViewProj);
            screenSpaceParticlePosition.xyz /= screenSpaceParticlePosition.w;

            // Only do depth buffer collisions if the particle is onscreen, otherwise assume no collisions
            if (pa.isSleeping == 0 && screenSpaceParticlePosition.x > -1 && screenSpaceParticlePosition.x < 1 && screenSpaceParticlePosition
                .y > -1 && screenSpaceParticlePosition.y < 1)
            {
                // Get the view space position of the depth buffer
                float3 viewSpacePosOfDepthBuffer = calcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(0, 0));

                // If the particle view space position is behind the depth buffer, but not by more than the collision thickness, then a collision has occurred
                if ((viewSpaceParticlePosition.z > viewSpacePosOfDepthBuffer.z) && (
                        viewSpaceParticlePosition.z < viewSpacePosOfDepthBuffer.z + CollisionThickness))
                {
                    // Generate the surface normal. Ideally, we would use the normals from the G-buffer as this would be more reliable than deriving them
                    float3 surfaceNormal;

                    // Take three points on the depth buffer
                    float3 p0 = viewSpacePosOfDepthBuffer;
                    float3 p1 = calcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(1, 0));
                    float3 p2 = calcViewSpacePositionFromDepth(screenSpaceParticlePosition.xy, int2(0, 1));

                    // Generate the view space normal from the two vectors
                    float3 viewSpaceNormal = normalize(cross(p2 - p0, p1 - p0));

                    // Transform into world space using the inverse view matrix
                    surfaceNormal = normalize(mul(-viewSpaceNormal, mInvView).xyz);

                    // The velocity is reflected in the collision plane
                    float3 newVelocity = reflect(pb.vel, surfaceNormal);

                    // Update the velocity and apply some restitution
                    pb.vel = 0.3 * newVelocity;

                    // Update the new collided position
                    vNewPosition = pb.pos + (pb.vel * DeltaTime);

                    pa.collisionCount++;
                }
            }
        }

        // Put particle to sleep if the velocity is small
        if (EnableSleepState && pa.collisionCount > 10 && length(pb.vel) < 0.01)
        {
            pa.isSleeping = 1;
        }

        // If the position is below the floor, let's kill it now rather than wait for it to retire
        if (vNewPosition.y < -1)
        {
            vNewPosition.y = 50.0f;
            //killParticle = true;
        }

        // Write the new position
        pb.pos = vNewPosition;

        // Calculate the the distance to the eye for sorting in the rasterization path
        float3 vec = vNewPosition - CamPos.xyz;
        pb.distanceToEye = length(vec);

        // The opacity is a function of the age
        float alpha = lerp(1, 0, saturate(fScaledLife - 0.8) / 0.2);
        pa.tintAndAlpha.a = pb.age <= 0 ? 0 : alpha;

        // Lerp the color based on the age
        /*float4 color0 = StartColor[ emitterIndex ];
        float4 color1 = EndColor[ emitterIndex ];
    
        pa.tintAndAlpha.rgb = lerp( color0, color1, saturate(5*fScaledLife) ).rgb;*/
        pa.tintAndAlpha.rgb = float3(1, 1, 1);

        if (ShowSleepingParticles && pa.isSleeping == 1)
        {
            pa.tintAndAlpha.rgb = float3(1, 0, 1);
        }

        /*// The emitter-based lighting models the emitter as a vertical cylinder
        float2 emitterNormal = normalize( vNewPosition.xz - EmitterLightingCenter.xz );

        // Generate the lighting term for the emitter
        float emitterNdotL = saturate( dot( SunDirection.xz, emitterNormal ) + 0.5 );
        pa.emitterNdotL = emitterNdotL;*/

        // Transform the velocity into view space
        float2 vsVelocity = mul(float4(pb.vel.xyz, 0), mView).xy;

        pa.velXY = vsVelocity;

        // Pack the view spaced position and radius into a float4 buffer
        float4 viewSpacePositionAndRadius;

        viewSpacePositionAndRadius.xyz = mul(float4(vNewPosition, 1), mView).xyz;
        viewSpacePositionAndRadius.w = radius;

        ViewSpacePositions[id.x] = viewSpacePositionAndRadius;

        // For streaked particles (the sparks), calculate the the max radius in XY and store in a buffer
        if (streaks)
        {
            float2 r2 = calcEllipsoidRadius(radius, pa.velXY);
            MaxRadiusBuffer[id.x] = max(r2.x, r2.y);
        }
        else
        {
            // Not a streaked particle so will have rotation. When rotating, the particle has a max radius of the centre to the corner = sqrt( r^2 + r^2 )
            MaxRadiusBuffer[id.x] = 1.41 * radius;
        }

        // Dead particles are added to the dead list for recycling
        if (pb.age <= 0.0f || killParticle)
        {
            pb.age = -1;
            DeadListToAddTo.Append(id.x);
        }
        else
        {
            // Alive particles are added to the alive list
            uint index = AliveIndexBuffer.IncrementCounter();
            AliveIndexBuffer[index] = float2(pb.distanceToEye, (float)id.x);

            uint dstIdx = 0;
#if defined (USE_GEOMETRY_SHADER)
            // GS path uses one vertex per particle
            InterlockedAdd(DrawArgs[0], 1, dstIdx);
#else
			// VS only path uses 6 indices per particle billboard
			InterlockedAdd( DrawArgs[ 0 ], 6, dstIdx );
#endif
        }
    }

    // Write the particle data back to the global particle buffer
    ParticleBufferA[id.x] = pa;
    ParticleBufferB[id.x] = pb;
}


// Reset 256 particles per thread group, one thread per particle
[numthreads(256,1,1)]
void CS_Reset(uint3 id : SV_DispatchThreadID)
{
    ParticleBufferA[id.x] = (ParticleA)0;
    ParticleBufferB[id.x] = (ParticleB)0;
}

#endif

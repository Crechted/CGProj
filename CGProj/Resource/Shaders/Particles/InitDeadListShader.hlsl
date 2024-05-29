#ifndef COMPUTE_PARTICLE
#define COMPUTE_PARTICLE

AppendStructuredBuffer<uint>	DeadListToAddTo		: register( u0 );

[numthreads(256,1,1)]
void CS_InitDeadList( uint3 id : SV_DispatchThreadID )
{
    DeadListToAddTo.Append( id.x );
}

#endif

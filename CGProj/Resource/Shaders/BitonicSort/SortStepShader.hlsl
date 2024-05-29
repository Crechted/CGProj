#ifndef BITONIC_SORT_STEP
#define BITONIC_SORT_STEP

#ifndef SORT_SIZE
#define SORT_SIZE 0
#endif

//--------------------------------------------------------------------------------------
// Structured Buffers
//--------------------------------------------------------------------------------------
RWStructuredBuffer<float2> Data : register( u0 );

//--------------------------------------------------------------------------------------
// Bitonic Sort Compute Shader
//--------------------------------------------------------------------------------------
cbuffer NumElementsCB : register( b0 )
{
int4 g_NumElements;
};

cbuffer SortConstants : register( b1 )
{
int4 job_params;
};

[numthreads(256, 1, 1)]
void BitonicSortStep(uint3 Gid : SV_GroupID,
    uint3 GTid : SV_GroupThreadID)
{
    int4 tgp;

    tgp.x = Gid.x * 256;
    tgp.y = 0;
    tgp.z = g_NumElements.x;
    tgp.w = min(512, max(0, g_NumElements.x - Gid.x * 512));

    uint localID = tgp.x + GTid.x; // calculate threadID within this sortable-array

    uint index_low = localID & (job_params.x - 1);
    uint index_high = 2 * (localID - index_low);

    uint index = tgp.y + index_high + index_low;
    uint nSwapElem = tgp.y + index_high + job_params.y + job_params.z * index_low;

    if (nSwapElem < tgp.y + tgp.z)
    {
        float2 a = Data[index];
        float2 b = Data[nSwapElem];

        if (a.x > b.x)
        {
            Data[index] = b;
            Data[nSwapElem] = a;
        }
    }
}
#endif

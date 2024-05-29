#include "SortLib.h"

#include "Core/Engine.h"

const int MAX_NUM_TG = 1024;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

SortLib::SortLib()
{
    engInst = &Engine::GetInstance();
}

SortLib::~SortLib()
{
    Destroy();
}

void SortLib::Destroy()
{
    if (sortStep) sortStep->Destroy();
    if (sortInner512) sortInner512->Destroy();
    if (sortLDS) sortLDS->Destroy();
    if (initArgs) initArgs->Destroy();

    if (pcbDispatchInfoBuf) pcbDispatchInfoBuf->Destroy();
    if (indirectSortArgsBuf) indirectSortArgsBuf->Destroy();

    SAFE_RELEASE(indirectSortArgsUAV);
}

typedef struct SortConstants
{
    int32_t x, y, z, w;
} int4;

void SortLib::InitSort()
{
    pcbDispatchInfoBuf = (new Buffer())->CreateBuffer<int4>();

    // create shaders

    sortStep = new Shader();
    sortStep->CreateShader(L"./Resource/Shaders/BitonicSort/SortStepShader.hlsl", SCompute, nullptr, (LPSTR)"BitonicSortStep");

    // Create inner sort shader
    const D3D10_SHADER_MACRO innerDefines[2] = {"SORT_SIZE", "512", nullptr, 0};
    sortInner512 = new Shader();
    sortInner512->CreateShader(L"./Resource/Shaders/BitonicSort/SortInnerShader.hlsl", SCompute, innerDefines, (LPSTR)"BitonicInnerSort");

    // create 
    const D3D10_SHADER_MACRO cs512Defines[2] = {"SORT_SIZE", "512", nullptr, 0};
    sortLDS = new Shader();
    sortLDS->CreateShader(L"./Resource/Shaders/BitonicSort/SortShader.hlsl", SCompute, cs512Defines, (LPSTR)"BitonicSortLDS");

    initArgs = new Shader();
    initArgs->CreateShader(L"./Resource/Shaders/BitonicSort/InitSortArgsShader.hlsl", SCompute, nullptr, (LPSTR)"InitDispatchArgs");

    indirectSortArgsBuf = (new Buffer())->CreateBuffer<uint32_t>(D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, 0,
        D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS, 4 * sizeof(uint32_t));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
    ZeroMemory(&uav, sizeof( uav ));
    uav.Format = DXGI_FORMAT_R32_UINT;
    uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav.Buffer.FirstElement = 0;
    uav.Buffer.NumElements = 4;
    uav.Buffer.Flags = 0;
    engInst->GetDevice()->CreateUnorderedAccessView(indirectSortArgsBuf->GetBuffer(), &uav, &indirectSortArgsUAV);
}

void SortLib::Sort(unsigned maxSize, ID3D11UnorderedAccessView* sortBufferUAV, ID3D11Buffer* itemCountBuffer)
{
    ID3D11UnorderedAccessView* prevUAV = nullptr;
    engInst->GetContext()->CSGetUnorderedAccessViews(0, 1, &prevUAV);

    ID3D11Buffer* prevCBs[] = {nullptr, nullptr};
    engInst->GetContext()->CSGetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

    ID3D11Buffer* cbs[] = {itemCountBuffer, pcbDispatchInfoBuf->GetBuffer()};
    engInst->GetContext()->CSSetConstantBuffers(0, ARRAYSIZE(cbs), cbs);

    // Write the indirect args to a UAV
    engInst->GetContext()->CSSetUnorderedAccessViews(0, 1, &indirectSortArgsUAV, nullptr);

    initArgs->BindShaders();
    engInst->GetContext()->Dispatch(1, 1, 1);

    engInst->GetContext()->CSSetUnorderedAccessViews(0, 1, &sortBufferUAV, nullptr);

    bool bDone = sortInitial(maxSize);

    int presorted = 512;
    while (!bDone)
    {
        bDone = sortIncremental(presorted, maxSize);
        presorted *= 2;
    }

#ifdef _DEBUG
    // this leaks resources somehow. Haven't looked into it yet.
    //manualValidate(maxSize, pUAV );
#endif

    // Restore previous state
    engInst->GetContext()->CSSetUnorderedAccessViews(0, 1, &prevUAV, nullptr);
    engInst->GetContext()->CSSetConstantBuffers(0, ARRAYSIZE(prevCBs), prevCBs);

    if (prevUAV) prevUAV->Release();

    for (size_t i = 0; i < ARRAYSIZE(prevCBs); i++)
        if (prevCBs[i]) prevCBs[i]->Release();
}

bool SortLib::sortInitial(unsigned maxSize)
{
    bool bDone = true;

    // calculate how many threads we'll require:
    //   we'll sort 512 elements per CU (threadgroupsize 256)
    //     maybe need to optimize this or make it changeable during init
    //     TGS=256 is a good intermediate value

    unsigned int numThreadGroups = ((maxSize - 1) >> 9) + 1;

    assert(numThreadGroups <= MAX_NUM_TG);

    if (numThreadGroups > 1) bDone = false;

    // sort all buffers of size 512 (and presort bigger ones)
    sortLDS->BindShaders();
    engInst->GetContext()->DispatchIndirect(indirectSortArgsBuf->GetBuffer(), 0);

    return bDone;
}

bool SortLib::sortIncremental(unsigned presorted, unsigned maxSize)
{
    bool bDone = true;
    sortStep->BindShaders();

    // prepare thread group description data
    unsigned int numThreadGroups = 0;

    if (maxSize > presorted)
    {
        if (maxSize > presorted * 2)
            bDone = false;

        unsigned int pow2 = presorted;
        while (pow2 < maxSize)
            pow2 *= 2;
        numThreadGroups = pow2 >> 9;
    }

    unsigned int nMergeSize = presorted * 2;
    for (unsigned int nMergeSubSize = nMergeSize >> 1; nMergeSubSize > 256; nMergeSubSize = nMergeSubSize >> 1)
    //	for( int nMergeSubSize=nMergeSize>>1; nMergeSubSize>0; nMergeSubSize=nMergeSubSize>>1 ) 
    {

        SortConstants sc = {};
        sc.x = nMergeSubSize;
        if (nMergeSubSize == nMergeSize >> 1)
        {
            sc.y = (2 * nMergeSubSize - 1);
            sc.z = -1;
        }
        else
        {
            sc.y = nMergeSubSize;
            sc.z = 1;
        }
        sc.w = 0;
        pcbDispatchInfoBuf->UpdateData(&sc);

        engInst->GetContext()->Dispatch(numThreadGroups, 1, 1);
    }

    sortInner512->BindShaders();
    engInst->GetContext()->Dispatch(numThreadGroups, 1, 1);

    return bDone;
}

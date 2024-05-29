#pragma once
#include "d3d11.h"

class Buffer;
class Shader;
class Engine;

class SortLib
{
public:
    SortLib();
    virtual ~SortLib();

    void Destroy();
    void InitSort();
    void Sort(unsigned int maxSize, ID3D11UnorderedAccessView* sortBufferUAV, ID3D11Buffer* itemCountBuffer);

protected:
    Engine* engInst;
    
    bool sortInitial(unsigned int maxSize);
    bool sortIncremental(unsigned int presorted, unsigned int maxSize);

    Shader* sortStep = nullptr;
    Shader* sortInner512 = nullptr;
    Shader* sortLDS = nullptr;
    Shader* initArgs = nullptr;

    Buffer* pcbDispatchInfoBuf = nullptr;
    Buffer* indirectSortArgsBuf = nullptr;

    ID3D11UnorderedAccessView* indirectSortArgsUAV = nullptr;
};

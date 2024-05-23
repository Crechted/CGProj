#pragma once
#include "d3d11.h"

class Engine;

class BlendState
{
public:
    BlendState();
    virtual ~BlendState();
    
    BlendState* CreateBlendState();
    BlendState* CreateBlendState(const D3D11_BLEND_DESC* blendStateDesc);
    BlendState* CreateAdditive();
    BlendState* CreateAlphaBlend();
    
    void Destroy();

    void Bind(const FLOAT blendFactor[4] = nullptr, UINT sampleMask = 0xffffffff);
    void UnBind(const FLOAT blendFactor[4] = nullptr, UINT sampleMask = 0xffffffff);
protected:
    Engine* engInst = nullptr;
    ID3D11BlendState* blendState;
};

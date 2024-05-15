#pragma once
#include "Shader.h"
#include "Core/Engine.h"

template <typename T_Data>
class Buffer
{
public:
    Buffer();
    virtual ~Buffer();

    Buffer* CreateBuffer(D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT bindFlags = D3D11_BIND_CONSTANT_BUFFER, UINT CPUFlags = 0,
        UINT miscFlags = 0);
    void UpdateData(const T_Data& data);
    void SetBuffer(uint32_t slot, ShaderType types);

    ID3D11Buffer* GetBuffer();
    void Destroy();

protected:
    T_Data dataBuffer;
    Engine* engInst;
    ID3D11Buffer* constBuf = nullptr;
};

#include "Buffer.hpp"

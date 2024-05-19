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
        UINT miscFlags = 0, UINT byteWidth = sizeof(T_Data), const D3D11_SUBRESOURCE_DATA* initData = nullptr);
    void UpdateData(const T_Data& data);
    void BindBuffer(uint32_t slot, ShaderType types);

    ID3D11Buffer* GetBuffer();
    ID3D11Buffer* const* GetBuffers() const;
    void Destroy();

protected:
    T_Data dataBuffer;
    Engine* engInst;
    ID3D11Buffer* constBuf = nullptr;
};

#include "Buffer.hpp"

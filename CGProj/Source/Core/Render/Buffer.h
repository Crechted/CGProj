#pragma once
#include "Shader.h"

class Buffer
{
public:
    Buffer();
    virtual ~Buffer();

    template <typename T_Data>
    Buffer* CreateBuffer(D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT bindFlags = D3D11_BIND_CONSTANT_BUFFER, UINT CPUFlags = 0,
        UINT miscFlags = 0, UINT byteWidth = sizeof(T_Data), const D3D11_SUBRESOURCE_DATA* initData = nullptr,
        UINT StructureByteStride = 0);

    template <typename T_Data>
    void UpdateData(const T_Data* data, int32_t idx = 0);
    void BindBuffers(uint32_t slot, uint32_t shaderTypes);

    ID3D11Buffer* GetBuffer(int32_t idx = 0);
    ID3D11Buffer* const* GetBuffers() const;
    void Destroy();

protected:
    Engine* engInst;
    Array<ID3D11Buffer*> buffers;
};

#include "Core/Engine.h"

template <typename T_Data>
Buffer* Buffer::CreateBuffer(D3D11_USAGE usage, UINT bindFlags, UINT CPUFlags, UINT miscFlags, UINT byteWidth,
    const D3D11_SUBRESOURCE_DATA* initData, UINT StructureByteStride)
{
    ID3D11Buffer* constBuf;
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = usage;
    constBufDesc.BindFlags = bindFlags;
    constBufDesc.CPUAccessFlags = CPUFlags;
    constBufDesc.MiscFlags = miscFlags;
    constBufDesc.ByteWidth = byteWidth;
    constBufDesc.StructureByteStride = StructureByteStride;
    auto hr = engInst->GetDevice()->CreateBuffer(&constBufDesc, initData, &constBuf);
    if (FAILED(hr)) return nullptr;
    buffers.insert(constBuf);
    return this;
}

template <typename T_Data>
void Buffer::UpdateData(const T_Data* data, int32_t idx)
{
    if (idx < buffers.size())
        engInst->GetContext()->UpdateSubresource(buffers[idx], 0, nullptr, data, 0, 0);
}

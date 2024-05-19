#pragma once
#include "Buffer.h"

template <typename T_Data>
Buffer<T_Data>::Buffer()
{
    engInst = &Engine::GetInstance();
}

template <typename T_Data>
Buffer<T_Data>::~Buffer()
{
    Destroy();
}

template <typename T_Data>
void Buffer<T_Data>::Destroy()
{
    if (constBuf) constBuf->Release();
}

template <typename T_Data>
Buffer<T_Data>* Buffer<T_Data>::CreateBuffer(D3D11_USAGE usage, UINT bindFlags, UINT CPUFlags, UINT miscFlags, UINT byteWidth,
    const D3D11_SUBRESOURCE_DATA* initData)
{
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = usage;
    constBufDesc.BindFlags = bindFlags;
    constBufDesc.CPUAccessFlags = CPUFlags;
    constBufDesc.MiscFlags = miscFlags;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = byteWidth;
    engInst->GetDevice()->CreateBuffer(&constBufDesc, initData, &constBuf);
    return this;
}

template <typename T_Data>
void Buffer<T_Data>::UpdateData(const T_Data& data)
{
    engInst->GetContext()->UpdateSubresource(constBuf, 0, nullptr, &data, 0, 0);
}

template <typename T_Data>
void Buffer<T_Data>::BindBuffer(uint32_t slot, ShaderType types)
{
    if (types & SVertex) engInst->GetContext()->VSSetConstantBuffers(slot, 1, &constBuf);
    if (types & SPixel) engInst->GetContext()->PSSetConstantBuffers(slot, 1, &constBuf);
    if (types & SGeometry) engInst->GetContext()->GSSetConstantBuffers(slot, 1, &constBuf);
    if (types & SCompute) engInst->GetContext()->CSSetConstantBuffers(slot, 1, &constBuf);
    if (types & SHull) engInst->GetContext()->HSSetConstantBuffers(slot, 1, &constBuf);
    if (types & SDomain) engInst->GetContext()->DSSetConstantBuffers(slot, 1, &constBuf);
}

template <typename T_Data>
ID3D11Buffer* Buffer<T_Data>::GetBuffer()
{
    return constBuf;
}

template <typename T_Data>
ID3D11Buffer* const* Buffer<T_Data>::GetBuffers() const
{
    return &constBuf;
}

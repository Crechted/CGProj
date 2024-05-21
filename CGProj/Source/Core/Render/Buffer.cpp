#include "Core/Render/Buffer.h"

Buffer::Buffer()
{
    engInst = &Engine::GetInstance();
}

Buffer::~Buffer()
{
    Destroy();
}

void Buffer::Destroy()
{
    for (const auto constBuf : buffers)
        if (constBuf) constBuf->Release();
}

void Buffer::BindBuffers(uint32_t slot, uint32_t shaderTypes)
{
    const uint32_t num = buffers.size();
    if (buffers.isEmpty()) return;
    if (shaderTypes & SVertex) engInst->GetContext()->VSSetConstantBuffers(slot, num, &buffers[0]);
    if (shaderTypes & SPixel) engInst->GetContext()->PSSetConstantBuffers(slot, num, &buffers[0]);
    if (shaderTypes & SGeometry) engInst->GetContext()->GSSetConstantBuffers(slot, num, &buffers[0]);
    if (shaderTypes & SCompute) engInst->GetContext()->CSSetConstantBuffers(slot, num, &buffers[0]);
    if (shaderTypes & SHull) engInst->GetContext()->HSSetConstantBuffers(slot, num, &buffers[0]);
    if (shaderTypes & SDomain) engInst->GetContext()->DSSetConstantBuffers(slot, num, &buffers[0]);
}

ID3D11Buffer* Buffer::GetBuffer(int32_t idx)
{
    return idx < buffers.size() ? buffers[idx] : nullptr;
}

ID3D11Buffer* const* Buffer::GetBuffers() const
{
    return buffers.isEmpty() ? nullptr : &buffers[0];
}

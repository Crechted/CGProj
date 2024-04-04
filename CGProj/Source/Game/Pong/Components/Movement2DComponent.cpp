#include "Movement2DComponent.h"

#include "../../../Core/Engine.h"
#include "../../../Core/Input/InputDevice.h"
#include "SimpleMath.h"

void Movement2DComponent::Initialize()
{
    if (!engInst) return;
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Movement2DComponent::OnKeyDown);
    engInst->GetInputDevice()->KeyUpDelegate.AddRaw(this, &Movement2DComponent::OnKeyUp);

    CreateConstBuffer();
}

void Movement2DComponent::Draw()
{
    SetConstBuffer();
}

void Movement2DComponent::Reload()
{
    offset = {0.0};
}

void Movement2DComponent::Update(float timeTick)
{
    UpdateConstBuffer(timeTick);
}

void Movement2DComponent::OnKeyDown(Keys key)
{
    AddInputByKey(key, speed);
}

void Movement2DComponent::OnKeyUp(Keys key)
{
    AddInputByKey(key, -speed);
}

void Movement2DComponent::AddInputByKey(Keys key, float vel)
{
    if (key == inputUp) delPosition.y += vel;
    if (key == inputLeft) delPosition.x -= vel;
    if (key == inputDown) delPosition.y -= vel;
    if (key == inputRight) delPosition.x += vel;
}

void Movement2DComponent::CreateConstBuffer()
{
    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(ConstData);

    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
}

void Movement2DComponent::SetConstBuffer()
{
    engInst->GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
}

void Movement2DComponent::CalcOffset(float timeTick)
{
    offset.x += delPosition.x * timeTick;
    offset.y += delPosition.y * timeTick;
}

void Movement2DComponent::UpdateConstBuffer(float timeTick)
{
    D3D11_MAPPED_SUBRESOURCE res = {};
    engInst->GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    CalcOffset(timeTick);
    ConstData data =
    {
        DirectX::XMFLOAT4(offset.x, offset.y, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &data, sizeof(ConstData));

    engInst->GetContext()->Unmap(constantBuffer, 0);
}

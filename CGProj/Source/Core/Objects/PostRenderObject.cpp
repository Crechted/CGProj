#include "Core/Objects/PostRenderObject.h"

#include "Core/Engine.h"


PostRenderObject::PostRenderObject(Vector2 viewportDim, Vector2 viewportPos, Vector2 viewportDepth)
{
    CreateViewport(viewportDim, viewportPos, viewportDepth);
}

PostRenderObject::PostRenderObject(const D3D11_VIEWPORT& viewport)
{
    SetViewport(viewport);
}

void PostRenderObject::Draw()
{
    if(!IsVisible()) return;
    Object::Draw();
}

void PostRenderObject::BindViewport() const
{
    engInst->GetContext()->RSSetViewports(1, &viewport);
}

void PostRenderObject::CreateViewport(Vector2 viewportDim, Vector2 viewportPos, Vector2 viewportDepth)
{
    viewport = {};
    viewport.TopLeftX = viewportPos.x;
    viewport.TopLeftY = viewportPos.y;
    viewport.Width = viewportDim.x;
    viewport.Height = viewportDim.y;
    viewport.MinDepth = viewportDepth.x;
    viewport.MaxDepth = viewportDepth.y;
}
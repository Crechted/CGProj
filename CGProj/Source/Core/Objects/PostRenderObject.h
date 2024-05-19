#pragma once
#include <d3d11.h>

#include "Object.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

class PostRenderObject : public Object
{
public:
    PostRenderObject(Vector2 viewportDim = Vector2(800, 800), Vector2 viewportPos = Vector2::Zero, Vector2 viewportDepth = Vector2(0.0f, 1.0f) );
    PostRenderObject(const D3D11_VIEWPORT& viewport);
    bool IsVisible() const { return visibility; }

    void Draw() override;
    void SetViewport(const D3D11_VIEWPORT& view) { viewport = view; }
    D3D11_VIEWPORT GetViewport() const { return viewport; };
    void BindViewport() const;
    void CreateViewport(Vector2 viewportDim = Vector2(800, 800), Vector2 viewportPos = Vector2::Zero, Vector2 viewportDepth = Vector2(0.0f, 1.0f));

protected:
    D3D11_VIEWPORT viewport = {};
    bool visibility = true;
};
#pragma once
#include "SimpleMath.h"


struct Vertex
{
    DirectX::SimpleMath::Vector4 position;
    DirectX::SimpleMath::Vector4 normal;
    DirectX::SimpleMath::Vector2 texture;
};

struct VertexNoTex
{
    DirectX::SimpleMath::Vector4 position;
    DirectX::SimpleMath::Vector4 color;
};

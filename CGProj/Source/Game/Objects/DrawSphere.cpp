#include "DrawSphere.h"

#include "Core/Components/DrawComponent.h"
#include "Core/Engine.h"
#include "Utils/Types.h"

DrawSphere::DrawSphere()
{
    triangleComp = CreateComponent<DrawComponent>();
}

void DrawSphere::Initialize()
{
    //triangleComp->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    auto defColor = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);

    auto phiStep = Pi / stackCount;
    auto thetaStep = 2.0f * Pi / sliceCount;

    auto curCol = doDifColor ? DirectX::XMFLOAT4(radius, radius, 0.0f, 1.0f) : defColor;
    triangleComp->AddVertex(DirectX::XMFLOAT4(0.0f, radius, 0.0f, 1.0f), curCol);
    for (int i = 1; i <= stackCount - 1; i++)
    {
        auto phi = i * phiStep;
        for (int j = 0; j <= sliceCount; j++)
        {
            auto theta = j * thetaStep;
            auto p = Vector3(
                static_cast<float>(radius * sin(phi) * cos(theta)),
                static_cast<float>(radius * cos(phi)),
                static_cast<float>(radius * sin(phi) * sin(theta))
                );
            curCol = doDifColor ? DirectX::XMFLOAT4(p.x, p.y, p.z, 1.0f) : defColor;
            triangleComp->AddVertex(DirectX::XMFLOAT4(p.x, p.y, p.z, 1.0f), curCol);
        }
    }
    curCol = doDifColor ? DirectX::XMFLOAT4(radius, -radius, 0.0f, 1.0f) : defColor;
    triangleComp->AddVertex(DirectX::XMFLOAT4(0.0f, -radius, 0.0f, 1.0f), curCol);

    if (drawUp)
        for (int i = 1; i <= sliceCount; i++)
        {
            triangleComp->AddIndex(0);
            triangleComp->AddIndex(i + 1);
            triangleComp->AddIndex(i);
        }

    auto baseIndex = 1;
    auto ringVertexCount = sliceCount + 1;
    for (int i = 0; i < stackCount - 2; i++)
    {
        for (int j = 0; j < sliceCount; j++)
        {
            if (drawFirstHalf)
            {
                triangleComp->AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
                triangleComp->AddIndex(baseIndex + i * ringVertexCount + j);
                triangleComp->AddIndex(baseIndex + i * ringVertexCount + j + 1);
            }
            if (drawSecondHalf)
            {
                triangleComp->AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
                triangleComp->AddIndex(baseIndex + (i + 1) * ringVertexCount + j + 1);
                triangleComp->AddIndex(baseIndex + i * ringVertexCount + j + 1);
            }
        }
    }

    if (drawDown)
    {
        auto southPoleIndex = triangleComp->GetNumVertices() - 1;
        baseIndex = southPoleIndex - ringVertexCount;
        for (int i = 0; i < sliceCount; i++)
        {
            triangleComp->AddIndex(southPoleIndex);
            triangleComp->AddIndex(baseIndex + i + 1);
            triangleComp->AddIndex(baseIndex + i);
        }
    }

    Object::Initialize();
}

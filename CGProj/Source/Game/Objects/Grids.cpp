#include "Grids.h"

#include <d3dcommon.h>

#include "Core/Components/DrawComponent.h"
#include "Core/Components/SceneComponent.h"


Grids::Grids()
{
    sceneComp = CreateComponent<SceneComponent>();
    coordAxis = CreateComponent<DrawComponent>();
    grid = CreateComponent<DrawComponent>();
}

void Grids::Initialize()
{
    CreateCoordinates();

    CreateGrid();

    Object::Initialize();
}

void Grids::CreateCoordinates()
{
    coordAxis->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    int32_t pointNum = 8;
    coordAxis->SetVertices(new DirectX::XMFLOAT4[pointNum]
    {
        DirectX::XMFLOAT4(0.0f, 0.1f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.1f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
    }, pointNum);
    int32_t idxCount = 6;
    coordAxis->SetIndexes(new int32_t[idxCount]{0, 1, 0, 2, 0, 3}, idxCount);
}

void Grids::CreateGrid()
{
    grid->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    auto color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    for (int32_t i = 0; i < numLineOnSide; i++)
    {
        for (int32_t j = 0; j < numLineOnSide; j++)
        {
            float x = cellSize*(i - numLineOnSide / 2);
            float z = cellSize*(j - numLineOnSide / 2);
            grid->AddVertex(DirectX::XMFLOAT4(x, 0.0f, z, 1.0f), color);
        }
    }
    for (int32_t i = 0; i < numLineOnSide; i++)
    {
        grid->AddIndex(i * numLineOnSide);
        grid->AddIndex((i + 1) * numLineOnSide - 1);
    }

    for (int32_t i = 0; i < numLineOnSide; i++)
    {
        grid->AddIndex(i);
        grid->AddIndex(numLineOnSide * (numLineOnSide - 1) + i);
    }
}

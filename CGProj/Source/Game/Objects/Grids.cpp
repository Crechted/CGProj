#include "Grids.h"

#include <d3dcommon.h>

#include "../../Components/TriangleComponent.h"
#include "../../Components/SceneComponent.h"


Grids::Grids()
{
    sceneComp = CreateComponent<SceneComponent>();
    coordAxis = CreateComponent<TriangleComponent>();
    grids = CreateComponent<TriangleComponent>();
}

void Grids::Initialize()
{
    coordAxis->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    int32_t pointNum = 8;
    coordAxis->SetPoints(new DirectX::XMFLOAT4[pointNum]
    {
        DirectX::XMFLOAT4(0.0f, 0.1f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.1f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.1f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
    }, pointNum);
    int32_t idxCount = 6;
    coordAxis->SetIndexes(new int32_t[idxCount]{0, 1, 0, 2, 0, 3}, idxCount);

    grids->topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    //pointNum = numLineOnSide * 2 * 2 * 2;
    pointNum = 44 * 2;
    /*auto points = new DirectX::XMFLOAT4[pointNum];
    for (int32_t i = 0; i < pointNum; i++)
    {
        points[i] = i % 2 == 0
                        ? DirectX::XMFLOAT4(cellSize * numLineOnSide * (i - numLineOnSide / 2), 0.0f,
                            cellSize * numLineOnSide * (i - numLineOnSide / 2), 1.0f)
                        : DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    grids->SetPoints(new DirectX::XMFLOAT4[pointNum]
    {
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 4, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 4, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 3, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 3, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 2, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 2, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 1, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 1, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, 0, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, 0, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 1, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 1, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 2, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 2, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 3, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 3, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 4, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 4, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),

        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 5, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 4, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 4, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 3, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 3, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 2, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 2, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 1, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 1, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-cellSize * 0, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 0, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 1, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 1, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 2, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 2, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 3, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 3, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 4, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 4, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(cellSize * 5, 0.0f, -cellSize * 5, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    }, pointNum);
    idxCount = 44;
    grids->SetIndexes(new int32_t[idxCount]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43},
        idxCount);

    Object::Initialize();
}

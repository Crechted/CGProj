#include "DrawBox.h"

#include "Box.h"
#include "Core/Components/DrawComponent.h"
#include "Core/Components/SceneComponent.h"

DrawBox::DrawBox(Vector3 position, Vector3 size, Vector4 color, D3D_PRIMITIVE_TOPOLOGY topology)
    : initSize(size)
      , initColor(color)
      , topology(topology)
{
    sceneComp = CreateComponent<SceneComponent>(nullptr, position);
    drawComp = CreateComponent<DrawComponent>();
}

void DrawBox::Initialize()
{
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Box::CreateDrawBoxByTopology(Vector3::Zero, initSize, initColor, vertices, indexes, topology);
    drawComp->topology = topology;
    drawComp->SetVertices(vertices);
    drawComp->SetIndexes(indexes);
    Object::Initialize();
}

void DrawBox::Draw()
{
    Object::Draw();
}

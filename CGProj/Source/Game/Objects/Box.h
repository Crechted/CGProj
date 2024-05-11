#pragma once
#include "SimpleMath.h"
#include "Core/Objects/Mesh.h"

using namespace DirectX::SimpleMath;
class BoxComponent;
class DrawComponent;

class Box : public Mesh
{
public:
    Box(const wchar_t* pathTex = nullptr,
        Vector3 position = Vector3::Zero,
        Vector3 size = Vector3(0.2f),
        Vector4 color = Vector4::One,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    void InitMesh() override;
    void InitBox(Vector3 size, D3D_PRIMITIVE_TOPOLOGY topology);
    static void CreateBoxByTopology(Vector3 size, Array<Vertex>& vertices, Array<int32_t>& indexes,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    static void CreateDrawBoxByTopology(Vector4 pos, Vector3 size, Vector4 color, Array<VertexNoTex>& vertices, Array<int32_t>& indexes,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    Vector3 initSize;
    Vector4 initColor;
protected:
    D3D_PRIMITIVE_TOPOLOGY topology;
private:
    static void GetIndexesByTopology(Array<int32_t>& indexes, D3D_PRIMITIVE_TOPOLOGY topology);
};

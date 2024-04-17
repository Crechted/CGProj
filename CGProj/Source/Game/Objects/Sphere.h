#pragma once
#include "Core/Objects/Mesh.h"

using namespace DirectX::SimpleMath;

class Sphere : public Mesh
{
public:
    Sphere(const wchar_t* pathTex = nullptr,
        Vector3 position = Vector3::Zero,
        float radius = 0.5f,
        int32_t sliceCount = 16,
        int32_t stackCount = 16,
        bool drawFirstHalf = true,
        bool drawSecondHalf = true,
        bool drawUp = true,
        bool drawDown = true
        );
    
    void Initialize() override;
    void InitSphere();
    
    static void CreateSphereByTopology(float radius, Array<Vertex>& vertices, Array<int32_t>& indexes,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    static void CreateDrawSphereByTopology(Vector3 pos, float radius, Vector4 color, Array<VertexNoTex>& vertices, Array<int32_t>& indexes,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

protected:
    Vector3 initPos = Vector3::Zero;
    float radius = 0.5f;
    int32_t sliceCount = 16;
    int32_t stackCount = 16;
    bool drawFirstHalf = true;
    bool drawSecondHalf = true;
    bool drawUp = true;
    bool drawDown = true;
};

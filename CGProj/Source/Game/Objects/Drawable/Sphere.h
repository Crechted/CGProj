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
    
    static void CreateSphere(Vector3 initPos, float radius, uint32_t stackCount, uint32_t sliceCount, Array<Vertex>& vertices, Array<int32_t>& indexes);
    
    static void CreateDrawSphere(Vector3 initPos, float radius, uint32_t stackCount, uint32_t sliceCount, Vector4 color, Array<VertexNoTex>& vertices, Array<int32_t>& indexes);

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

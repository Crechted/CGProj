#pragma once
#include "SimpleMath.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Objects/Object.h"

class TextureComponent;
class BlendState;
using namespace DirectX::SimpleMath;
class Buffer;
class Shader;

struct ParticleData
{
    Vector3 pos;
    Vector3 vel;
};

struct HandlerData
{
    int32_t GroupDim;
    uint32_t MaxParticles;
    float TotalTime;
    float DeltaTime;
};

class ParticleSystem : public Object
{
public:
    ParticleSystem(int32_t particleCount = 1000000, const Transform& transform = Transform::Identity, SceneComponent* parentComp = nullptr);
    void Destroy() override;
    void CreateShader();
    void CreateBuffer();
    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;

    SceneComponent* GetSceneComponent() const { return sceneComp; }
    TextureComponent* GetTextureComponent() const { return textureComp; }

protected:
    Shader* particleShader;
    Shader* particleComputeShader;
    Buffer* particlesBuf;
    Buffer* handlerBuf;
    TextureComponent* textureComp;
    ID3D11ShaderResourceView* particlesSRV;
    ID3D11UnorderedAccessView* particlesUAV;
    BlendState* blendState;
    SceneComponent* sceneComp;

    int32_t particleCount;
    int32_t groupSizeX;
    int32_t groupSizeY;
};

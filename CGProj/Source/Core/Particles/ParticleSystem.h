#pragma once
#include "SimpleMath.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Objects/Object.h"

enum class Keys;
class SortLib;
class TextureComponent;
class BlendState;
using namespace DirectX::SimpleMath;
class Buffer;
class Shader;

struct EmitterData
{
    Vector4 pos;
    Vector4 vel;
    Vector4 posVar;

    int32_t maxParticlesThisFrame;
    float lifeSpan;
    float startSize;
    float endSize;

    float velVar;
    float mass;
    int32_t ID;
    int32_t streaks;

    alignas(16) int32_t texID;
};

struct ParticleBuf
{
    Vector4 params[3];
};

struct HandlerData
{
    Matrix mInvProjView;
    Matrix mInvProj;
    Matrix mInvView;
    Matrix mView;
    Matrix mProj;
    Matrix mViewProj;

    Vector3 CamPos;
    uint32_t MaxParticles;

    Vector2 ScreenDimensions;
    float TotalTime;
    float DeltaTime;

    float CollisionThickness;
    int CollideParticles;
    int ShowSleepingParticles;
    int EnableSleepState;
};

struct ListCount
{
    alignas(16) uint32_t count;
};

struct IndexBufferElement
{
    float distance; // distance squared from the particle to the camera
    float index;    // global index of the particle
};

class ParticleSystem : public Object
{
public:
    ParticleSystem(int32_t maxParticleCount = 1000000, const Transform& transform = Transform::Identity,
        SceneComponent* parentComp = nullptr);
    void Destroy() override;
    void Initialize() override;
    void Reload() override;
    void Update(float timeTick) override;
    void PreDraw() override;
    void Draw() override;
    void SetEmitter(const EmitterData& emit) { emitter = emit; }

    SceneComponent* GetSceneComponent() const { return sceneComp; }
    TextureComponent* GetTextureComponent() const { return textureComp; }

protected:
    bool doCollide = false;
    EmitterData emitter;

    SortLib* sortLib = nullptr;
    
    Shader* renderShader;
    Shader* emitShader;
    Shader* simulateShader;
    Shader* resetShader;
    Shader* particleComputeShader;
    Shader* initDeadListShader;

    Buffer* particlesBuffs;
    Buffer* indirectDrawArgsBuf;
    Buffer* handlerBuf;
    Buffer* emitterBuf;
    Buffer* deadListCountBuf;
    Buffer* activeListCountBuf;

    TextureComponent* textureComp;
    ID3D11ShaderResourceView* particlesA_SRV;
    ID3D11ShaderResourceView* particlesB_SRV;
    ID3D11ShaderResourceView* viewSpaceParticlePosSRV;
    ID3D11ShaderResourceView* maxRadiusSRV;
    ID3D11ShaderResourceView* aliveIndexBufferSRV;

    ID3D11UnorderedAccessView* particlesA_UAV;
    ID3D11UnorderedAccessView* particlesB_UAV;
    ID3D11UnorderedAccessView* viewSpaceParticlePosUAV;
    ID3D11UnorderedAccessView* maxRadiusUAV;
    ID3D11UnorderedAccessView* aliveIndexBufferUAV;

    ID3D11UnorderedAccessView* deadListUAV;
    ID3D11UnorderedAccessView* indirectDrawArgsUAV;

    ID3D11Texture2D* randomTexture;
    ID3D11ShaderResourceView* randomTextureSRV;

    BlendState* blendState;

    ID3D11SamplerState* samWrapLinear = nullptr;
    ID3D11SamplerState* samClampLinear = nullptr;

    SceneComponent* sceneComp;

    bool needReset = true;
    int32_t maxParticleCount;
    int32_t particleCount;
    int32_t particleEmitRate;
    int32_t curParticleToEmit;


    void Emit(int32_t numToEmit);
    void Simulate(ID3D11ShaderResourceView* depthSRV, bool useGS = true);
    void Sort();
    void FillRandomTexture();
    void InitDeadList();
    void InitSort();
    void CreateShaders();
    void CreateBuffers();
    void CreateBlendState();
    void CreateSamplers();
    void InitDefaultEmitter();

    void OnKeyDown(Keys key);
};

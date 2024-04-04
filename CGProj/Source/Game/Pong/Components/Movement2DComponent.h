#pragma once
#include <d3d11.h>
//#include <DirectXMath.h>

#include "SimpleMath.h"
#include "../../../Components/GameComponent.h"
#include "../../../Core/Input/Keys.h"

enum class Keys;

struct ConstData
{
    DirectX::XMFLOAT4 offset;
    DirectX::XMFLOAT4 color;
};

class Movement2DComponent : public GameComponent
{
public:
    void Initialize() override;
    void Draw() override;
    void Reload() override;
    void Update(float timeTick) override;

    DirectX::SimpleMath::Vector2 GetOffset() const {return offset;}
    ID3D11Buffer* constantBuffer;
    D3D11_BUFFER_DESC constBufDesc;

    float speed = 1;

    Keys inputUp = Keys::W;
    Keys inputDown = Keys::S;
    Keys inputLeft = Keys::A;
    Keys inputRight = Keys::D;
protected:
    virtual void AddInputByKey(Keys key, float vel);
    
    void CreateConstBuffer();
    virtual void UpdateConstBuffer(float timeTick);
    void SetConstBuffer();
    virtual void CalcOffset(float timeTick);
    virtual void OnKeyDown(Keys key);
    virtual void OnKeyUp(Keys key);
    
    DirectX::SimpleMath::Vector2 delPosition;
    DirectX::SimpleMath::Vector2 offset;
};

#include "Components/TriangleComponent.h"
#include "Core/Game.h"

void AddTriangle(Game* game, DirectX::XMFLOAT4* points)
{
    const auto Component1 = new TriangleComponent();        
    Component1->SetPoints(points, 3);    
    Component1->Initialize();
    game->gameComponents.insert(Component1);
}

int main()
{
    Game* game = &Game::GetGame();
    game->Initialize();
    
    AddTriangle(game, new DirectX::XMFLOAT4[3]{        
        DirectX::XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, 0.2f, 1.0f, 1.0f),
    });

    AddTriangle(game, new DirectX::XMFLOAT4[3]{        
        DirectX::XMFLOAT4(-0.2f, 0.2f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-0.2f, 0.5f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.2f, 1.0f, 1.0f),
    });

    AddTriangle(game, new DirectX::XMFLOAT4[3]{        
        DirectX::XMFLOAT4(-0.2f, -0.2f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-0.2f, -0.5f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.2f, 1.0f, 1.0f),
    });

    AddTriangle(game, new DirectX::XMFLOAT4[3]{        
        DirectX::XMFLOAT4(0.2f, -0.2f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.2f, -0.5f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.2f, 1.0f, 1.0f),
    });
    
    
    game->Run();
    
    return 0;
}


/*DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),*/
#include "Components/TriangleComponent.h"
#include "Core/Game.h"

void AddTriangle(Game *game, DirectX::XMFLOAT4 *points, int32_t count, int32_t numElemets = 2) {
  const auto Component1 = new TriangleComponent();
  Component1->SetPoints(points, count);
  Component1->numElements = numElemets;
  Component1->Initialize();
  game->gameComponents.insert(Component1);
}

int main() {
  Game *game = &Game::GetGame();
  game->Initialize();

  AddTriangle(game, new DirectX::XMFLOAT4[6]{
                  DirectX::XMFLOAT4(-0.1f, 0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, 0.4f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, 0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, 0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, 0.2f, 1.0f, 1.0f),
              }, 6);

  AddTriangle(game, new DirectX::XMFLOAT4[6]{
                  DirectX::XMFLOAT4(-0.2f, 0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
                  DirectX::XMFLOAT4(-0.5f, 0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
                  DirectX::XMFLOAT4(-0.35f, 0.7f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
              }, 6);

  AddTriangle(game, new DirectX::XMFLOAT4[8]{
                  DirectX::XMFLOAT4(-0.2f, -0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.2f, 0.5f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(-0.5f, -0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(-0.5f, -0.5f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.0f, -1.0f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
              }, 8);

  /*AddTriangle(game, new DirectX::XMFLOAT4[3]{
                  DirectX::XMFLOAT4(0.2f, -0.2f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.2f, -0.5f, 1.0f, 1.0f),
                  DirectX::XMFLOAT4(0.5f, -0.2f, 1.0f, 1.0f),
              }, 3);*/

  game->Run();

  return 0;
}


/*      DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),*/

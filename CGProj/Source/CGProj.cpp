#include "Components/SceneComponent.h"
#include "Components/TriangleComponent.h"
#include "Core/Game.h"
#include "Game/Components/BoxComponent.h"
#include "Game/Objects/Box.h"
#include "Game/Objects/Grids.h"
#include "Game/Planets/Planet.h"
#include "Game/Planets/Components/PlanetMoveComponent.h"

int main()
{

    Game* game = &Game::GetGame();
    //game->CreateComponent<SceneComponent>();
    game->CreateObjects<Grids>();
    Planet* planet = game->CreateObjects<Planet>();
    planet->meshComponent->center = Vector3(1.0, 1.0, 0.0);
    planet->meshComponent->size = Vector3(0.4);
    planet->moveComp->isRotationAround = true;
    /*Box* box2 = game->CreateObjects<Box>();
    box2->center = Vector3(0.0, 0.0, 1.5);
    box2->color = Vector3(1.0f, 0.0f, 0.0f);*/

    game->Initialize();
    game->Run();

    return 0;
}

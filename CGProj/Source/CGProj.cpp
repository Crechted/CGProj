#include "Core/Game.h"
#include "Game/Components/BoxComponent.h"
#include "Game/Objects/Grids.h"
#include "Game/Planets/Planet.h"
#include "Game/Planets/Components/PlanetMoveComponent.h"

int main()
{

    Game* game = &Game::GetGame();
    //game->CreateComponent<SceneComponent>();
    game->CreateObjects<Grids>();
    game->CreateCamera();
    game->camera->sceneComponent->initRotation = Vector3(0.0f, 0.0f, 0.0f);


    Planet* planet2 = game->CreateObjects<Planet>();
    planet2->meshComponent->initPosition = Vector3(-1.0f, 0.0f, 0.0f);
    planet2->meshComponent->size = Vector3(0.1f);
    planet2->meshComponent->color = Vector3(1.0f, 0.0f, 0.0f);
    planet2->moveComp->rotationAroundAxis =  Vector3(0.0f, 5.0f, 0.0f);
    planet2->moveComp->isRotationAround = true;
    planet2->moveComp->moveByVector =  Vector3(0.0f, 0.0f, 0.2f);
    planet2->moveComp->isMoveByVector = true;
    
    Planet* planet = game->CreateObjects<Planet>();
    planet->meshComponent->initPosition = Vector3(1.0, 0.0, 0.0);
    planet->meshComponent->size = Vector3(0.2f);
    planet->meshComponent->color = Vector3(1.0f, 1.0f, 0.0f);
    planet->moveComp->rotationAroundAxis =  Vector3(0.0f, 10.0f, 0.0f);
    planet->moveComp->isRotationAround = true;
    planet->moveComp->moveByVector =  Vector3(0.5f, 1.0f, 0.0f);
    planet->moveComp->isMoveByVector = false;
    
    planet->AddSputnik(planet2, 2.3);

    game->Initialize();
    game->Run();
    return 0;
}

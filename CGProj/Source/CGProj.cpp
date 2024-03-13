#include "Core/Game.h"
#include "Game/Camera.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Objects/Grids.h"
#include "Game/Planets/Planet.h"
#include "Game/Planets/Components/PlanetMoveComponent.h"

void Create4Window(Game* game)
{
    int32_t space = 50;
    int32_t size = GetSystemMetrics(SM_CYSCREEN) / 2 - space;
    int32_t offset = (GetSystemMetrics(SM_CXSCREEN) - 3 * size) / 2;
    game->AddWindow(size, size, offset + space + 2 * size, space + size, ViewType::OrtXOY);
    game->AddWindow(size, size, offset + space, size + space, ViewType::OrtYOZ);
    game->AddWindow(size, size, offset + size + space, space, ViewType::OrtXOZ);
    game->AddWindow(size, size, offset + size + space, size + space);
}

int main()
{

    Game* game = &Game::GetGame();
    auto grids = game->CreateObject<Grids>();

    Planet* planet = game->CreateObject<Planet>();
    planet->meshComponent->initPosition = Vector3(0.5f, 0.0f, 0.0f);
    planet->meshComponent->radius = 0.2f;
    planet->meshComponent->color = Vector3(1.0f, 1.0f, 0.0f);
    planet->moveComp->rotationAroundAxis = Vector3(0.0f, 20.0f, 0.0f);
    planet->moveComp->isRotationAround = true;
    planet->moveComp->isMoveByDirection = true;
    planet->meshComponent->drawFirstHalf = false;
    planet->keyToPoses = Keys::D1;

    Planet* planet2 = game->CreateObject<Planet>();
    planet2->meshComponent->initPosition = Vector3(-0.2f, 0.0f, 0.0f);
    planet2->meshComponent->radius = 0.1f;
    planet2->meshComponent->color = Vector3(1.0f, 0.0f, 0.0f);
    planet2->moveComp->rotationAroundAxis = Vector3(0.0f, 50.0f, 0.0f);
    planet2->moveComp->isRotationAround = true;
    planet2->moveComp->isMoveByDirection = true;
    planet2->meshComponent->doDifColor = true;
    planet2->keyToPoses = Keys::D2;

    //planet->AddSputnik(planet2, 2.3);    

    Planet* planet3 = game->CreateObject<Planet>();
    planet3->meshComponent->initPosition = Vector3(0.2f, 1.0f, 1.0f);
    planet3->meshComponent->initRotation = Vector3(45.0f, 0.0f, 45.0f);
    planet3->meshComponent->color = Vector3(0.2f, 1.0f, 1.0f);
    planet3->meshComponent->doDifColor = true;
    planet3->moveComp->isRotationAround = true;
    planet3->moveComp->rotationAroundAxis = Vector3(0.0f, -7.0f, 1.0f);
    planet3->keyToPoses = Keys::D3;
    //planet3->meshComponent->drawDown = false;
    //planet3->meshComponent->drawUp = false;
    planet3->AddSputnik(2.0f, Keys::D4);

    //Create4Window(game);
    game->Initialize();
    game->Run();
    return 0;
}

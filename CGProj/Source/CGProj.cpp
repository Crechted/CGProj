#include "Core/Components/MeshComponent.h"
#include "Core/Engine.h"
#include "Core/Components/CollisionComponent.h"
#include "Core/Objects/Mesh.h"
#include "Game/Camera.h"
#include "Game/Catomary/Objects/CatomaryObjects.h"
#include "Game/Catomary/Objects/Pawn.h"
#include "Game/Components/BoxComponent.h"
#include "Game/Components/DrawSphereComponent.h"
#include "Game/Components/SpringArmComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Objects/Box.h"
#include "Game/Objects/Grids.h"
#include "Game/Objects/SkySphere.h"
#include "Game/Planets/Components/PlanetMoveComponent.h"

int main()
{
    Engine* engine = &Engine::GetInstance();
    //auto grids = engine->CreateObject<Grids>(1.0f, 41);

    auto pawn = engine->CreateObject<Pawn>();
    pawn->GetSphereComponent()->GetMesh()->SetCollisionVisibility(true);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(-10.0f, -0.1f, -10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(-10.0f, -0.1f, 10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(10.0f, -0.1f, 10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(10.0f, -0.1f, -10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);

    /*auto camera = game->CreateCamera();
    camera->springArmComp->initPosition = Vector3(10.0f, 2.0f, 10.0f);
    camera->springArmComp->initRotation = Vector3(0.0f);
    camera->movementComp->speed = 3.0f;
    camera->springArmComp->attachOnlyTranslation = true;
    camera->viewType = ViewType::General;
    game->SetCurCamera(camera);*/

    const auto mesh = engine->CreateObject<CatomaryObjects>("Resource/Meshes/Piram.obj");
    mesh->GetSceneComponent()->initPosition = Vector3(2.0f,1.0f,0.0f);

    /*auto mesh = engine->CreateObject<Mesh>("Resource/Meshes/GunLow2.fbx");
    mesh->SetCollisionVisibility(true);
    auto box = engine->CreateObject<Box>(nullptr, Vector3(2.0f, 2.0f, 0.0f), Vector3(0.2, 0.1, 0.2));
    auto spher = engine->CreateComponent<SphereComponent>(L"Resource/Textures/Putin.dds", Vector3(4.0f, 2.0f, 0.0f));
    auto sky = engine->CreateObject<SkySphere>();
    spher->GetMesh()->SetCollisionVisibility(true);
    box->SetCollisionVisibility(true);*/
    engine->Initialize();
    engine->Run();

    return 0;
}

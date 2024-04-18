#include "Core/Components/MeshComponent.h"
#include "Core/Engine.h"
#include "Core/Components/CollisionComponent.h"
#include "Core/Objects/Mesh.h"
#include "Game/Camera.h"
#include "Game/Catomary/Components/PlayerMoveComponent.h"
#include "Game\Catomary\Objects\CatamaryObjects.h"
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

    auto pawn = engine->CreateObject<Pawn>(5.0f, Vector3(-3.0f, 0.5f, 0.0f));
    pawn->GetSphereComponent()->GetMesh()->SetCollisionVisibility(true);
    pawn->GetPlayerMoveComponent()->speed = 2.0f;
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(-10.0f, -0.05f, -10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(-10.0f, -0.05f, 10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(10.0f, -0.05f, 10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    engine->CreateComponent<BoxComponent>(L"Resource/Textures/grass.png", Vector3(10.0f, -0.05f, -10.0f), Vector3(20.0f, 0.1f, 20.0f))->
            GetMeshCollision()->SetCollisionEnabled(false);
    

    const auto mesh = engine->CreateObject<CatamaryObjects>("Resource/Meshes/Piram.obj");
    mesh->GetSceneComponent()->initPosition = Vector3(2.0f,0.0f,0.0f);
    mesh->GetSceneComponent()->initScale =  Vector3(2.0);
    //mesh->GetSceneComponent()->initRotation = Vector3(-90.0f, 0.0f, 3.0f);

    
    auto gun = engine->CreateObject<CatamaryObjects>("Resource/Meshes/GunLow2.fbx");
    gun->SetCollisionVisibility(true);
    gun->GetSceneComponent()->initScale =  Vector3(0.02);
    gun->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, 3.0f);

    //gun->GetSceneComponent()->initRotation = Vector3(-90.0f, 0.0f, 3.0f);


    auto chair = engine->CreateObject<CatamaryObjects>("Resource/Meshes/Chair/Chair and Table.fbx", L"Resource/Meshes/Chair/Texture/Chair and table_Base_color.png");
    chair->SetCollisionVisibility(true);
    //chair->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, 3.0f);
    //chair->GetSceneComponent()->initRotation = Vector3(-90.0f, 0.0f, 3.0f);
    chair->GetSceneComponent()->initScale =  Vector3(5.0f);

    auto gear = engine->CreateObject<CatamaryObjects>("Resource/Meshes/Gear/Gear2.obj", L"Resource/Meshes/Gear/textures/Gear_2_BaseColor.png");
    gear->SetCollisionVisibility(true);
    gear->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, -3.0f);
    //gear->GetSceneComponent()->initRotation = Vector3(-90.0f, 0.0f, 3.0f);
    gear->GetSceneComponent()->initScale =  Vector3(0.05f);
    
    auto sky = engine->CreateObject<SkySphere>();
    sky->SetCollisionVisibility(false);
    sky->SetCollisionEnabled(false);
    engine->Initialize();
    engine->Run();

    return 0;
}

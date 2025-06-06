﻿#include "Core/Components/MeshComponent.h"
#include "Core/Engine.h"
#include "Core/WinDisplay.h"
#include "Core/Components/CollisionComponent.h"
#include "Core/Components/TextureComponent.h"
#include "Core/Objects/Mesh.h"
#include "Core/Render/RenderTarget.h"
#include "Game/Camera.h"
#include "Game/Catomary/Components/PlayerMoveComponent.h"
#include "Game/Catomary/Objects/CatamariObjects.h"
#include "Game/Catomary/Objects/CatamariPawn.h"
#include "Game/Components/BoxComponent.h"
#include "Game/Components/LightComponents/DirectionalLightComponent.h"
#include "Game/Components/MovementComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Components/SpringArmComponent.h"
#include "Game/Components/LightComponents/PointLightComponent.h"
#include "Game/Components/LightComponents/SpotLightComponent.h"
#include "Game/Objects/PostRenderGameMap.h"
#include "Game/Objects/PostRenderImage.h"
#include "Game/Objects/Drawable/Box.h"
#include "Game/Objects/SkySphere.h"
#include "Game/Objects/Spectator.h"


int main()
{
    Engine* engine = &Engine::GetInstance();
    //auto grids = engine->CreateObject<Grids>(1.0f, 41);

    auto pawn = engine->CreateObject<CatamariPawn>(7.0f, Vector3(-3.0f, 0.5f, 0.0f));
    pawn->GetSphereComponent()->GetMesh()->SetCollisionVisibility(true);
    pawn->GetPlayerMoveComponent()->speed = 2.0f;
    /*auto spec = engine->CreateObject<Spectator>(0.0f, Vector3(0.0f, 25.0f, 25.0f));
    spec->GetMovementComponent()->speed = 5.0f;*/

    const auto planeMesh1 = engine->CreateComponent<BoxComponent>(L"Resource/Textures/Ice.jpg", Vector3(-10.0f, -0.05f, -10.0f),
        Vector3(20.0f, 0.1f, 20.0f))->GetMesh();
    const auto planeMesh2 = engine->CreateComponent<BoxComponent>(L"Resource/Textures/Ice.jpg", Vector3(-10.0f, -0.05f, 10.0f),
        Vector3(20.0f, 0.1f, 20.0f))->GetMesh();
    const auto planeMesh3 = engine->CreateComponent<BoxComponent>(L"Resource/Textures/Ice.jpg", Vector3(10.0f, -0.05f, 10.0f),
        Vector3(20.0f, 0.1f, 20.0f))->GetMesh();
    const auto planeMesh4 = engine->CreateComponent<BoxComponent>(L"Resource/Textures/Ice.jpg", Vector3(10.0f, -0.05f, -10.0f),
        Vector3(20.0f, 0.1f, 20.0f))->GetMesh();

    auto mat = planeMesh1->GetTextureComponent()->GetMaterial();
    mat.specularPower = 2.0f;
    mat.specularScale = 0.05f;
    planeMesh1->SetMaterial(mat);
    planeMesh2->SetMaterial(mat);
    planeMesh3->SetMaterial(mat);
    planeMesh4->SetMaterial(mat);

    const auto mesh = engine->CreateObject<CatamariObjects>("Resource/Meshes/Piram.obj");
    mesh->GetSceneComponent()->initPosition = Vector3(2.0f, 0.0f, 0.0f);
    mesh->GetSceneComponent()->initScale = Vector3(2.0);

    auto gun = engine->CreateObject<CatamariObjects>("Resource/Meshes/GunLow2.fbx");
    gun->GetSceneComponent()->initScale = Vector3(0.02f);
    gun->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, 3.0f);
    gun->GetCollision()->SetCollisionVisibility(true);

    auto chair = engine->CreateObject<CatamariObjects>("Resource/Meshes/Chair/Chair and Table.fbx",
        L"Resource/Meshes/Chair/Texture/Chair and table_Base_color.png");
    chair->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, 1.5f);
    chair->GetSceneComponent()->initScale = Vector3(5.0f);
    chair->GetCollision()->SetCollisionVisibility(true);

    auto gear = engine->CreateObject<CatamariObjects>("Resource/Meshes/Gear/Gear2.obj",
        L"Resource/Meshes/Gear/textures/Gear_2_BaseColor.png");
    gear->GetSceneComponent()->initPosition = Vector3(0.0f, 0.5f, -3.0f);
    gear->GetSceneComponent()->initScale = Vector3(0.05f);
    gear->GetCollision()->SetCollisionVisibility(true);

    auto ball = engine->CreateObject<CatamariObjects>("Resource/Meshes/Basketball_ball.obj");

    /*auto sky = engine->CreateObject<SkySphere>();
    sky->SetCollisionVisibility(false);
    sky->SetCollisionEnabled(false);*/
    Transform trans{Vector3::One};
    engine->CreateComponent<PointLightComponent>(trans, 4.0f, Vector4(1.0f, 0.0f, 0.0f, 0.0f) * 10.0f, true);

    engine->CreateComponent<DirectionalLightComponent>();

    Transform trans1{Vector3(0.0f, 1.0f, 2.0f)};
    engine->CreateComponent<PointLightComponent>(trans1, 6.0f, Vector4(0.0f, 0.0f, 1.0f, 0.0f) * 10.0f, true);

    Transform trans2{Vector3(-3.0f, 4.0f, -2.0f), Vector3(-45.0f, -135.0f, 0.0f)};
    engine->CreateComponent<SpotLightComponent>(trans2, 6.0f, 15.0f, Vector4(0.0f, 2.0f, 0.5f, 0.0f), true);

    auto transper1 = engine->CreateObject<Box>(nullptr, Vector3(-2.0f, 1.0f, -3.0f), Vector3(0.5f));
    transper1->SetOpacity(0.5f);
    auto transper2 = engine->CreateObject<Box>(nullptr, Vector3(-2.0f, 1.0f, -4.0f), Vector3(0.7f));
    transper2->SetOpacity(0.7f);

    auto particleSys = engine->CreateObject<ParticleSystem>(100000, Transform({0.0f, 25.0f, 0.0f}));
    particleSys->GetTextureComponent()->SetTexture(L"Resource/Textures/snowflake.png");

    //auto img = engine->CreateObject<PostRenderGameMap>( L"./Resource/Shaders/ImgShader.hlsl", nullptr, Vector2(200, 200), Vector2(20.0, 20.0f));

    //particleSys->GetSceneComponent()->AttachTo(spec->GetSpringArmComponent());
    engine->useCascadeShadow = true;
    engine->SetRenderType(RenderType::Deferred);
    engine->Initialize();

    engine->Run();
    return 0;
}

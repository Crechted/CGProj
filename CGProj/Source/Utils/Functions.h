#pragma once


#include "Game/Components/SpringArmComponent.h"
#include "Core/Components/MeshComponent.h"
#include "Core/Engine.h"
#include "Game/Camera.h"
#include "Game/Components/DrawSphereComponent.h"
#include "Game/Planets/Planet.h"
#include "Game/Planets/Components/PlanetMoveComponent.h"

static void Create4Screen(Engine* game, bool OnOneWindow = true)
{
    if (!OnOneWindow)
    {
        int32_t space = 50;
        int32_t size = GetSystemMetrics(SM_CYSCREEN) / 2 - space;
        int32_t offset = (GetSystemMetrics(SM_CXSCREEN) - 3 * size) / 2;
        game->AddWindow(size, size, offset + space + 2 * size, space + size, ViewType::OrtXOY);
        game->AddWindow(size, size, offset + space, size + space, ViewType::OrtYOZ);
        game->AddWindow(size, size, offset + size + space, space, ViewType::OrtXOZ);
        game->AddWindow(size, size, offset + size + space, size + space);
    }
    else
    {
        int32_t space = 50;
        int32_t size = GetSystemMetrics(SM_CYSCREEN) / 2 - space;
        int32_t offset = (GetSystemMetrics(SM_CXSCREEN) - 3 * size) / 2;
        auto cam1 = game->CreateCamera(ViewType::OrtXOY);
        auto cam2 = game->CreateCamera(ViewType::OrtYOZ);
        auto cam3 = game->CreateCamera(ViewType::OrtXOZ);
        auto cam4 = game->CreateCamera(ViewType::General);
        cam1->springArmComp->attachOnlyTranslation = true;
        cam2->springArmComp->attachOnlyTranslation = true;
        cam3->springArmComp->attachOnlyTranslation = true;
        cam4->springArmComp->attachOnlyTranslation = true;
        game->AddWindow(-1, -1, -1, -1);
    }
}

static Planet* CreatePlanet(Vector3 pos, Vector3 rot, Vector3 Scale, bool doDifCol, Vector3 col, float rad, bool isRot,
    Vector3 rotationAroundAxis,
    bool drawAll, Keys keyToPos)
{
    Engine* game = &Engine::GetInstance();
    Planet* planet = game->CreateObject<Planet>();
    planet->meshComponent->initPosition = pos;
    planet->meshComponent->initRotation = rot;
    planet->meshComponent->initScale = Scale;
    planet->meshComponent->radius = rad;
    planet->meshComponent->color = col;
    planet->meshComponent->doDifColor = doDifCol;
    planet->moveComp->isRotationAround = isRot;
    planet->moveComp->rotationAroundAxis = rotationAroundAxis;
    planet->meshComponent->drawFirstHalf = drawAll;
    planet->keyToPoses = keyToPos;
    return planet;
}

static void SimulatePlanets()
{
    Planet* planet1 = CreatePlanet(Vector3(0.0f, 2.0f, 0.0f), Vector3(0.0f), Vector3(1.0f), false, Vector3(1.0, 0.8, 0.2), 4.0f, true,
        Vector3(0.30f, 45.0f, 0.0f), true, Keys::D1);
    planet1->meshComponent->attachOnlyTranslation = true;
    planet1->moveComp->speed = 1.f;

    Planet* planet2 = CreatePlanet(Vector3(-7.0f, 0.0f, 0.0f), Vector3(0.0f), Vector3(1.0f), false, Vector3(0.0, 1.0, 0.2), 1.0f, true,
        Vector3(0.0f, -6.0f, 0.0f), false, Keys::D2);
    planet2->moveComp->speed = 1.0f;
    planet1->AddSputnik(planet2);

    Planet* planet3 = CreatePlanet(Vector3(22.0f, 0.0f, 4.0f), Vector3(00.0f), Vector3(1.0f), false, Vector3(1.0, 0.4, 0.9), 1.0f, true,
        Vector3(0.0f, 5.0f, 0.0f), false, Keys::D3);
    planet1->AddSputnik(planet3);
    planet3->moveComp->speed = -10.0f;

    Planet* planet4 = CreatePlanet(Vector3(7.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.6f), false, Vector3(0.2, 0.4, 0.9),
        1.0f, true, Vector3(0.0f, -10.0f, 0.0f), false, Keys::D4);
    planet4->moveComp->speed = 5.0f;
    planet3->AddSputnik(planet4);
    // planet4->moveComp->isMove = false;

    Planet* planet5 = CreatePlanet(Vector3(00.0f, .0f, 4.5f), Vector3(0.0f, 0.0f, 00.0f), Vector3(0.2f), true, Vector3(0.2, 0.8, 0.3),
        1.0f, true, Vector3(-0.0f, -30.0f, 00.0f), false, Keys::D5);
    planet5->moveComp->speed = -10.f;
    planet4->AddSputnik(planet5);

    Planet* planet6 = CreatePlanet(Vector3(00.0f, 4.5f, 4.5f), Vector3(45.0f, 0.0f, 0.0f), Vector3(0.2f), true, Vector3(0.2, 0.8, 0.3),
        1.0f, true, Vector3(-0.0f, 10.0f, 00.0f), true, Keys::D6);
    planet6->moveComp->speed = -4.f;
    planet3->AddSputnik(planet6);
    //planet5->moveComp->isMove = false;
}

#include "BoxComponent.h"

#include "../../Core/Engine.h"
#include "../Objects/Box.h"

BoxComponent::BoxComponent(const wchar_t* pathTex, Vector3 position, Vector3 size, Vector4 color)
{
    if (mesh) mesh->Destroy();
    mesh = new Box(pathTex, position, size, color);
}

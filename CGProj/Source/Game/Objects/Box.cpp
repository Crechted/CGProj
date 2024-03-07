#include "Box.h"

#include "../../Components/SceneComponent.h"
#include "../../Components/TriangleComponent.h"
#include "../Components/BoxComponent.h"

Box::Box()
{
    boxComponent = CreateComponent<BoxComponent>();
}

void Box::Initialize()
{
    Object::Initialize();
}

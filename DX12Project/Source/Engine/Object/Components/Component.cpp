#include "Component.h"

class Scene* Component::GetScene() const
{
    return nullptr;
}

void Component::Register(class Scene* InScene)
{
    CurrentScene = InScene;
}

Component* Component::GetParent() const
{
    return nullptr;
}

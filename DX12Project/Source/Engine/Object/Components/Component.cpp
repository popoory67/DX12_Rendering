#include "Component.h"
#include "Scene.h"
#include "Entity.h"

Component::Component(Scene* InScene, Component* InParent)
    : CurrentScene(InScene)
    , Parent(InParent)
{

}

Entity* Component::GetOwner() const
{
    return Owner;
}

Scene* Component::GetScene() const
{
    return CurrentScene;
}

void Component::Register(Scene* InScene)
{
    CurrentScene = InScene;
}

Component* Component::GetParent() const
{
    return Parent;
}

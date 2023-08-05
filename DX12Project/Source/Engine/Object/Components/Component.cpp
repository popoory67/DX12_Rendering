#include "Component.h"
#include "Scene.h"

Component::Component(Scene* InScene, Component* InParent)
    : CurrentScene(InScene)
    , Parent(InParent)
{

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

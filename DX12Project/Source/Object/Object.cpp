#include "stdafx.h"
#include "Object.h"

Component::Component()
{

}

Component::~Component()
{

}

Object::Object()
{
	TransformData = new Transform();
}
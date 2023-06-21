#pragma once
#include "Class.h"

struct UploadData
{

};

class Component : public BaseClass
{
public:
	Component() = default;
	virtual ~Component() = default;

protected:
	virtual void Update() {}
	
protected:
	std::string Name;

	int NumFramesDirty = 3;
};
#pragma once
#include "Class.h"

struct BUploadData
{

};

class BComponent : public BClass
{
public:
	BComponent() = default;
	virtual ~BComponent() = default;

protected:
	virtual void Update() {}
	
protected:
	std::string Name;

	int NumFramesDirty = 3;
};
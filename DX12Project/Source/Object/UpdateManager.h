#pragma once
#include "GameTimer.h"

class UpdateManager
{
public:
	UpdateManager();
	~UpdateManager();

	// Data update
	//void Update();
	void UpdateTransform(const GameTimer& InTimer);

private:
	// frame resources
//	std::unique_ptr<FrameResource> CurFrameResource;
	std::map<unsigned, class Object*> Objects;

};


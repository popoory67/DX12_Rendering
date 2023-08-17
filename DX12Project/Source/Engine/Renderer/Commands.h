#pragma once
#include "CommandList.h"
#include <vector>

struct RHICommand_Primitive : public RHICommandBase<RHICommand_Primitive>
{
	RHICommand_Primitive() = delete;
	RHICommand_Primitive(std::vector<struct MeshRenderBatch>&& InBatch);
	virtual ~RHICommand_Primitive();

	void Execute(const RHICommandList& InCmdList) override
	{

	}

private:
	std::vector<struct MeshRenderBatch> Batches;
};

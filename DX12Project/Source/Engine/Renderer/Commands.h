#pragma once
#include "CommandList.h"
#include <vector>

using VertexStream = std::vector<struct Vertex>;

struct RHICommand_Primitive : public RHICommandBase<RHICommand_Primitive>
{
	RHICommand_Primitive() = delete;
	RHICommand_Primitive(VertexStream&& InStream, unsigned int InSize, unsigned int InStride);
	virtual ~RHICommand_Primitive();

	void Execute(const RHICommandList& InCommandList) override;

private:
	VertexStream StreamResource;
	unsigned int Size;
	unsigned int Stride;
};

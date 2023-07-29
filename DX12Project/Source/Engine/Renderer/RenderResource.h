#pragma once

class RenderResource
{
public:
	virtual void Initialize();
	virtual void Release();
};

class VertexBuffer : public RenderResource
{
	virtual void Initialize() override
	{

	}
};
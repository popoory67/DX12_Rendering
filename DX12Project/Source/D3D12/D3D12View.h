#pragma once
#include "D3D12Resource.h"

#define MAX_RENDER_TARGETS 8

class D3D12View
{
public:
	D3D12View() = delete;
	D3D12View(class D3D12Descriptor* InDescriptor);
	virtual ~D3D12View();

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle(UINT64 InIndex = 0) const;

protected:
	class D3D12Descriptor* Descriptor;
};

class D3D12RenderTargetView : public RHIRenderTargetInfo, public D3D12View
{
	friend class D3D12Viewport;

	using Parent = RHIRenderTargetInfo;

public:
	D3D12RenderTargetView() = delete;
	D3D12RenderTargetView(D3D12Resource* InResource, class D3D12Descriptor* InDescriptor);
	virtual ~D3D12RenderTargetView() = default;

	void Reset() override;
};

class D3D12DepthStencilView : public RHIDepthStencilInfo, public D3D12View
{
	using Parent = RHIDepthStencilInfo;

public:
	D3D12DepthStencilView() = delete;
	D3D12DepthStencilView(D3D12Resource* InResource, class D3D12Descriptor* InDescriptor);
	virtual ~D3D12DepthStencilView() = default;
};

template <>
struct TD3D12Types<RHIRenderTargetInfo>
{
	using ResourceType = D3D12RenderTargetView;
};
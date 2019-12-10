#pragma once
#include <d3d12.h>

using namespace Microsoft::WRL;

class D3D12PipelineState
{
public:
	D3D12PipelineState() = delete;
	D3D12PipelineState(class D3D12Device* InDevice, DXGI_FORMAT InBackBufferFormat, DXGI_FORMAT InDepthStencilFormat);
	~D3D12PipelineState();

	ComPtr<ID3D12PipelineState> Get() { return PipelineState; }
	ID3D12PipelineState* GetInterface() { return PipelineState.Get(); }

	void SetVertexShader(D3D12_SHADER_BYTECODE& InShaderByte);
	void SetPixelShader(D3D12_SHADER_BYTECODE& InShaderByte);

private:
	ComPtr<ID3D12PipelineState> PipelineState;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
};
#include "D3DUtil.h"
#include <comdef.h>
#include "D3D12Resource.h"

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
	ErrorCode(hr),
	FunctionName(functionName),
	Filename(filename),
	LineNumber(lineNumber)
{
}

std::wstring DxException::ToString()const
{
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}

namespace D3DUtil
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC ParsePSOStream(D3D12_PIPELINE_STATE_STREAM_DESC& InStreamDesc)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        for (SIZE_T offset = 0, size = 0; offset < InStreamDesc.SizeInBytes; offset += size)
        {
            BYTE* pStream = static_cast<BYTE*>(InStreamDesc.pPipelineStateSubobjectStream) + offset;
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type = *reinterpret_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE*>(pStream);

            switch (type)
            {
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE:
                psoDesc.pRootSignature = *reinterpret_cast<ID3D12RootSignature**>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::pRootSignature);
                break;
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
                psoDesc.VS = *reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::VS)*>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::VS);
                break;
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS:
                psoDesc.PS = *reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::PS)*>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::PS);
                break;
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS:
                psoDesc.DS = *reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DS)*>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DS);
                break;
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS:
                psoDesc.HS = *reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::HS)*>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::HS);
                break;
            case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS:
                psoDesc.GS = *reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::GS)*>(pStream);
                size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::GS);
                break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS:
            //    pCallbacks->ASCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM2::AS)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM2::AS);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS:
            //    pCallbacks->MSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM2::MS)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM2::MS);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT:
            //    pCallbacks->StreamOutputCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::StreamOutput)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::StreamOutput);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND:
            //    pCallbacks->BlendStateCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::BlendState)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::BlendState);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK:
            //    pCallbacks->SampleMaskCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::SampleMask)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::SampleMask);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER:
            //    pCallbacks->RasterizerStateCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::RasterizerState)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::RasterizerState);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL:
            //    pCallbacks->DepthStencilStateCb(*reinterpret_cast<CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1:
            //    pCallbacks->DepthStencilState1Cb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DepthStencilState)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DepthStencilState);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT:
            //    pCallbacks->InputLayoutCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::InputLayout)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::InputLayout);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE:
            //    pCallbacks->IBStripCutValueCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::IBStripCutValue)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::IBStripCutValue);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY:
            //    pCallbacks->PrimitiveTopologyTypeCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::PrimitiveTopologyType)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::PrimitiveTopologyType);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS:
            //    pCallbacks->RTVFormatsCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::RTVFormats)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::RTVFormats);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT:
            //    pCallbacks->DSVFormatCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DSVFormat)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DSVFormat);
            //    break;
            //case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC:
            //    pCallbacks->SampleDescCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::SampleDesc)*>(pStream));
            //    size = sizeof(CD3DX12_PIPELINE_STATE_STREAM::SampleDesc);
            //    break;
            }
        }
        return psoDesc;
    }

};
#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include "MathHelper.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class D3DDeviceBase
{
public:
	D3DDeviceBase() {}
	virtual ~D3DDeviceBase() {}
};

class D3D12Device : public D3DDeviceBase
{
public:
	D3D12Device(HWND InWindowHandle);
	virtual ~D3D12Device();

	ComPtr<ID3D12Device> GetDevice() { return Device; }
	ComPtr<IDXGIFactory4> GetDxgi() { return DxgiFactory; }

	HWND GetWindowHandle() { return MainWindowHandle; }

private:
	void CreateDevice();

private:
	// COM(Component Object Model)
	// The COM object is like a C++ class.
	// DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술

	// 언어 독립성 : C++을 C++로써만 사용할 수 있게 함
	// 하위 호환성 : 내부와 연결하여 사용할 수 있게 함

	// 말하자면, Com 인터페이스를 하나의 클래스 오브젝트로 생각하고 사용하면 된다는 의미

	// ComPtr is like a smart pointer.
	ComPtr<ID3D12Device> Device; // Render를 위한 기능을 담고 있는 인터페이스

	// DirectX Graphics Infrastructure : 공통 그래픽 API
	ComPtr<IDXGIFactory4> DxgiFactory; // Swap chain 인터페이스 생성, 디스플레이 어댑터(= 그래픽카드) 정보

	HWND MainWindowHandle = nullptr;
};
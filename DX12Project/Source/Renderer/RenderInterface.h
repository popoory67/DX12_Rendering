#pragma once
#include "D3D12RenderInterface.h"

class RenderInterface
{
public:
	RenderInterface() = delete;
	RenderInterface(class D3D12RenderInterface* InInterface, class SceneRenderer* InSceneRenderer);
	~RenderInterface() = default;

	void CreateResource(class D3D12Resource* InResource, std::optional<D3D12_RESOURCE_DESC> InDesc = {}, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateResource(class D3D12Resource* InResource, UINT64 InByteSize, std::optional<D3D12_CLEAR_VALUE> InValue = {});
	void CreateDefaultBuffer(class D3D12DefaultResource* InResource, const void* InInitData, UINT64 InByteSize);

	// TODO : 테스트 용으로 임시 batch 인터페이스 구현. 나중에 멀티 쓰레드로 다시 구현하기
	template <class T>
	static void AddCommand(class CommandBase<T>* InCommand);

	void AddScene(class Scene* InScene) const;
	
	//template<typename Type>
	//void CreateUploadResource(class D3D12UploadResource<Type>* InResource, UINT InElementCount)
	//{
	//	GetInterface()->CreateUploadResource<Type>(InResource, InElementCount);
	//}

private:

	static std::unique_ptr<class D3D12RenderInterface> CurrentInterface;
	static std::unique_ptr<class SceneRenderer> CurrnetSceneRenderer;
};

template <class T>
void RenderInterface::AddCommand(CommandBase<T>* InCommand)
{
	
}

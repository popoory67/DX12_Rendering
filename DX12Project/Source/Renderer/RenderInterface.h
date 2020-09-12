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

	// TODO : �׽�Ʈ ������ �ӽ� batch �������̽� ����. ���߿� ��Ƽ ������� �ٽ� �����ϱ�
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

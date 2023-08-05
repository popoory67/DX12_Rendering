#pragma once
#include "Component.h"
#include <windows.h>
#include <minwindef.h>
#include <unordered_map>
#include <any>

class RenderComponent : public Component
{
	using Parent = Component;

public:
	RenderComponent() = delete;
	RenderComponent(class Scene* InScene, Component* InParent);
	virtual ~RenderComponent() = default;

	template<typename DataType>
	void AddShaderBinding(std::string InName, DataType InData)
	{
		Bindings.emplace(std::make_pair(InName, InData));
	}

	template<typename DataType>
	void UpdateShaderBinding(std::string InName, DataType InData)
	{
		auto it = Bindings.find(InName);
		if (it != Bindings.cend())
		{
			it->second = InData;
		}
	}

	UINT GetBindingSize();

private:
	std::unordered_map<std::string, std::any> Bindings;
};
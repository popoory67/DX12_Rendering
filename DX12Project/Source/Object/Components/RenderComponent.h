#pragma once
#include "Component.h"

// For using data mirroring
// The data must be used to connect from Component and Render update
struct BRenderUploadData : public BUploadData
{

};

class BRenderComponent : public BComponent
{
public:
	BRenderComponent() = default;
	virtual ~BRenderComponent() = default;

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

	std::map<std::string, std::any> Bindings;
};
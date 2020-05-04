#pragma once

class Component
{
public:
	Component();
	virtual ~Component();

protected:
	std::string Name;

	int NumFramesDirty = 3;
};

class RenderComponent : public Component
{
public:
	RenderComponent();
	virtual ~RenderComponent();

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

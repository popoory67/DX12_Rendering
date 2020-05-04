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
		std::unique_ptr<Binding<DataType>> bound = std::make_unique<Binding<DataType>>();
		bound->dataType = typeid(DataType);
		bound->value = InData;

		Bindings.emplace(std::make_pair(InName, bound));
	}

	template<typename DataType>
	void UpdateShaderBinding(std::string InName, DataType InData)
	{
		auto it = Bindings.find(InName);
		if (it != Bindings.cend())
		{
			static_cast<Binding<DataType>>(it->second)->value = InData;
		}
	}

	UINT GetBindingSize();

private:
	struct BindingInterface
	{
		type_info dataType;

		virtual ~BindingInterface() = default;
	};

	template<typename DataType>
	struct Binding : public BindingInterface
	{
		DataType value;
	};

	std::map<std::string, std::unique_ptr<BindingInterface>> Bindings;
};

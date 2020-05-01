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
	void AddShaderBatch(std::string InName, DataType InData)
	{
		Batch batch;
		batch.dataType = typeid(DataType);
		batch.data = static_cast<void*>(InData);

		Batches.emplace(std::make_pair(InName, batch));
	}

	template<typename DataType>
	void UpdateShaderBatch(std::string InName, DataType InData)
	{
		auto it = Batches.find(InName);
		if (it != Batches.cend())
		{
			it->second.data = static_cast<void*>(InData);
		}
	}

	static UINT GetBatchSize();

private:
	struct Batch
	{
		size_t dataType;
		void* data;
	};

	static std::map<std::string, Batch> Batches;
};

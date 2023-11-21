#pragma once
#include "ThreadBase.h"
#include "RenderResource.h"
#include <type_traits>
#include <unordered_map>
#include <memory>

namespace GraphicsPipelineState
{
	struct Key
	{
		Key() = delete;
		Key(size_t InVertexHash, size_t InFragmentHash)
			: VertexShader(InVertexHash)
			, FragmentShader(InFragmentHash)
		{
		}

		size_t VertexShader;
		size_t FragmentShader;
	};

	struct Hash
	{
		std::size_t operator()(const Key& InDesc) const
		{
			return InDesc.VertexShader ^ InDesc.FragmentShader;
		}
	};

	struct HashCompare
	{
		bool operator()(const Key& lhs, const Key& rhs) const
		{
			return lhs.VertexShader == rhs.VertexShader &&
				lhs.FragmentShader == rhs.FragmentShader;
		}
	};
};

class PipelineState
{
public:
	PipelineState() = default;
	virtual ~PipelineState() = default;

private:
	ShaderBinding* VS;
	ShaderBinding* FS;
};

class PSOLibrary : public Task
{
public:
	PSOLibrary() = default;
	~PSOLibrary() = default;

	bool Init() override;
	void Run() override;
	void Stop() override;

	std::weak_ptr<PipelineState> GetPSOCache(const GraphicsPipelineState::Key& InDesc);

private:
	bool Load(size_t InVertexHash, size_t InFragmentHash);
	void Save(size_t InVertexHash, size_t InFragmentHash);
	void CreateAndAddCache(const GraphicsPipelineState::Key& InDesc);
	std::weak_ptr<PipelineState> FindCache(const GraphicsPipelineState::Key& InDesc);

private:
	std::unordered_map<GraphicsPipelineState::Key, std::shared_ptr<PipelineState>, GraphicsPipelineState::Hash, GraphicsPipelineState::HashCompare> PipelineStateCaches;

};
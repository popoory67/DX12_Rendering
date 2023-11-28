#pragma once
#include "ThreadBase.h"
#include "RenderResource.h"
#include <type_traits>
#include <unordered_map>
#include <memory>

class PipelineStateCache;

namespace GraphicsPipelineState
{
	struct Key
	{
		Key() = default;
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

		bool operator()(size_t lhs, size_t rhs) const
		{
			return lhs == rhs;
		}
	};

	using PSOStream = std::vector<char>;
	using PSOList = std::unordered_map<size_t, PSOStream>;

	std::size_t GetHash(const Key& InKey);
	std::wstring GetPSOFileName(std::size_t InKey);
	PSOStream GetPSOCache(const Key& InKey);
	void AddPSOCache(size_t InKey, const PSOStream& InPSO);
	void Save(size_t InKey, const PSOStream& InPSO);
};

class PipelineStateCache
{
public:
	PipelineStateCache() = default;
	virtual ~PipelineStateCache() = default;

protected:
	ShaderBinding* VS;
	ShaderBinding* FS;
};

class PSOLibrary : public Task
{
	using Parent = Task;

public:
	PSOLibrary() = default;
	~PSOLibrary() = default;

	bool Init() override;
	void Run() override;
	void Stop() override;

private:
	GraphicsPipelineState::PSOStream Load(const std::wstring& InPath);
	void Save(size_t InKey, GraphicsPipelineState::PSOStream& InPSO);
};
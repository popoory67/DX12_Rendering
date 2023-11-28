#include "PipelineState.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace GraphicsPipelineState
{
    class PSOService
    {
        friend class PSOLibrary;

    public:
        ~PSOService() = default;

        static PSOService& Get()
        {
            static PSOService instance;
            return instance;
        }

        PSOStream FindCache(size_t InKey)
        {
            // lock

            auto it = PipelineStateCaches.find(InKey);
            if (it != PipelineStateCaches.cend())
            {
                return it->second;
            }
            return {};
        }

        void AddPSOCache(size_t InKey, const PSOStream& InPSO)
        {
            // lock

            PipelineStateCaches.insert(std::pair<size_t, PSOStream>{ InKey, InPSO });
        }

        void Save(size_t InKey, const PSOStream& InPSO)
        {
            const auto& it = FindCache(InKey);
            if (!it.empty())
            {
                return;
            }
            AddPSOCache(InKey, InPSO);
            SaveList.push(std::pair<size_t, PSOStream>{ InKey, InPSO });
        }

        std::queue<std::pair<size_t, PSOStream>>& GetSaveList()
        {
            return SaveList;
        }

    private:
        PSOService() = default;

    private:
        GraphicsPipelineState::PSOList PipelineStateCaches;
        std::queue<std::pair<size_t, PSOStream>> SaveList;
    };

    std::size_t GetHash(const Key& InKey)
    {
        GraphicsPipelineState::Hash hashGenerator{};
        return hashGenerator(InKey);
    }

    std::wstring GetPSOFileName(std::size_t InKey)
    {
        std::wstring fileName = std::to_wstring(InKey);

        return fileName;
    }

    PSOStream GetPSOCache(const Key& InKey)
    {
        size_t hashKey = GetHash(InKey);
        return PSOService::Get().FindCache(hashKey);
    }

    void AddPSOCache(size_t InKey, const PSOStream& InPSO)
    {
        PSOService::Get().AddPSOCache(InKey, InPSO);
    }

    void Save(size_t InKey, const PSOStream& InPSO)
    {
        PSOService::Get().Save(InKey, InPSO);
    }
};

bool PSOLibrary::Init()
{
    // Load offline files
    // Most of PSO caches would be loaded this phase.
    // A PSO cache that wasn't saved before is just a tiny exception.
    //GraphicsPipelineState::PSOStream pso = Load(Util::GetAssetFullPath(L"/PSO/PSO.cache"));

    //// Data to PSOService
    //GraphicsPipelineState::PSOService::Get().AddPSOCache(pso);

    std::wstring path = Util::GetAssetFullPath(L"/PSO/");

    try 
    {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) 
        {
            for (const auto& entry : std::filesystem::directory_iterator(path)) 
            {
                auto filename = entry.path().filename(); 
                {
                    GraphicsPipelineState::PSOStream psoStream = Load(Util::GetAssetFullPath(L"PSO/" + filename.wstring()));

                    size_t hashKey = std::stoull(filename.wstring());
                    GraphicsPipelineState::AddPSOCache(hashKey, std::move(psoStream));
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& err) 
    {
        std::cerr << "Filesystem error: " << err.what() << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "General error: " << e.what() << std::endl;
    }
    return true;
}

void PSOLibrary::Run()
{
    while (!bStop)
    {
        auto& SaveList = GraphicsPipelineState::PSOService::Get().GetSaveList();

        // Check if PSO exists with RHI.
        while (!SaveList.empty())
        {
            std::pair<size_t, GraphicsPipelineState::PSOStream> pso = SaveList.front();
            SaveList.pop();

            Save(pso.first, pso.second);
        }
    }
}

void PSOLibrary::Stop()
{
    Parent::Stop();

    // Save
    //for (auto& pso : SaveList)
    //{
    //    Save();
    //}
}

GraphicsPipelineState::PSOStream PSOLibrary::Load(const std::wstring& InPath)
{
    // If there is not PSO which has not been loaded,
    // it has to be created as a new PSO.
    std::ifstream file(InPath.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    GraphicsPipelineState::PSOStream buffer(size);
    if (!file.read(buffer.data(), size))
    {
        throw std::runtime_error("Failed to read PSO file.");
    }
    file.close();

    return buffer;
}

void PSOLibrary::Save(size_t InKey, GraphicsPipelineState::PSOStream& InPSO)
{
    std::wstring fileName = GraphicsPipelineState::GetPSOFileName(InKey);
    std::wstring path = L"/PSO/" + fileName + L".cache";

    // When this program is closing, the save list have to save to the PSO cache file.
    std::ofstream outFile(Util::GetAssetFullPath(path), std::ios::binary);
    outFile.write(reinterpret_cast<char*>(InPSO.data()), InPSO.size());
    outFile.close();
}
#include "DataAssetLoader.h"

#include <Windows.h>

#include "WaveFrontLoader.h"

UDataAssetLoader::UDataAssetLoader(const std::string& InAssetFolder)
{
    AssetRootPath = GetExePath() + InAssetFolder;
}

std::string UDataAssetLoader::GetExePath() const
{
    char szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串//
    std::string path = szFilePath;
    return path;
}

UTexture* UDataAssetLoader::LoadTexture(const std::string& Path, ETextureType TextureType)
{
    const std::string AbsPath = GetAbsPath(Path);

    UTexture* Ret = GetAssetFromCache<UTexture>(AbsPath);
    if(!Ret)
    {
        Ret = new UTexture(AbsPath, TextureType);
        AssetMap.emplace(AbsPath, reinterpret_cast<char*>(Ret));
    }
    return Ret; 
}

FModelData* UDataAssetLoader::LoadModel(const std::string& Path)
{
    const std::string AbsPath = GetAbsPath(Path);
    FModelData* Ret = GetAssetFromCache<FModelData>(AbsPath);
    if(!Ret)
    {
        Ret = UWaveFrontLoader::LoadFromFile(AbsPath);
        AssetMap.emplace(AbsPath, reinterpret_cast<char*>(Ret));
    }
    return Ret;
}

std::string UDataAssetLoader::GetAbsPath(const std::string& Path) const
{
    return AssetRootPath + Path;
}

template <typename T>
T* UDataAssetLoader::GetAssetFromCache(const std::string& AbsPath) const
{
    const auto DataIterator = AssetMap.find(AbsPath);
    if(AssetMap.end() == DataIterator) return nullptr;
    return reinterpret_cast<T*>(DataIterator->second);
}

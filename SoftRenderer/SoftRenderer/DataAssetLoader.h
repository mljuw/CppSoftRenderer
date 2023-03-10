#pragma once
#include <unordered_map>

#include "UTexture.h"


class UDataAssetLoader
{
protected:
    std::unordered_map<std::string, char *> AssetMap;

    std::string AssetRootPath;
    
public:
    UDataAssetLoader(const std::string& InAssetFolder);
    
    UTexture* LoadTexture(const std::string& Path, ETextureType TextureType);

    FModelData* LoadModel(const std::string& Path);

protected:
    std::string GetAbsPath(const std::string& Path) const;

    template<typename T>
    T* GetAssetFromCache(const std::string& AbsPath) const;

     
    std::string GetExePath() const;
};

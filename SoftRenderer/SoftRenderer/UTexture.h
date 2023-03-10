#pragma once
#include "BaseStruct.h"
#include "FrameBuffer.h"

enum class ETextureType : unsigned char
{
    sRGB,
    LinearColor,
    Normal,
};

enum class ETextureSampleType : unsigned char
{
    Nearest,
    BiLinear
};

class UTexture
{
public:
    FSize Size;
    FSize MaxIndex;
    
private:
    unsigned char * ImageData = nullptr;

    UMyBuffer<FVector4>* CustomData = nullptr;

    bool bCustomData = false;

    int ChannelSize = 4;

    ETextureType TextureType;
public:
    UTexture(const FSize& InSize, const FVector4& DefaultValue);

    UTexture(const std::string FilePath, ETextureType InType = ETextureType::sRGB);

    ~UTexture();

    void ClearCustomData() const;

    void SetColor(int X, int Y, const FVector4& Color);

    FVector4 GetColor(int X, int Y);
    
    FVector4 SampleWithNearest(const FVector2& UV);
    
    FVector4 SampleWithBiLinear(const FVector2& UV);
    
    void ClampMaxIndex(int& X, int& Y);
};

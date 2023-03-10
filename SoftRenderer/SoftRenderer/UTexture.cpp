#include "UTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Define.h"
#include "stb_image.h"

UTexture::UTexture(const FSize& InSize,  const FVector4& DefaultValue)
{
    bCustomData = true;
    Size = InSize;
    MaxIndex = Size - 1;
    CustomData = new UMyBuffer<FVector4>(Size, DefaultValue);
    TextureType = ETextureType::sRGB;
}

UTexture::UTexture(const std::string FilePath, ETextureType InType)
{
    /*
    * 描述:载入图像，支持的图像文件格式包括JPEG、PNG、TGA、BMP、PSD、GIF、HDR、PIC、PNM
    * 
    * 参数：
    *   filename：图像文件名
    *   x：获取图像宽
    *   y：获取图像高
    *   channels_in_file：获取图像通道数
    *   desired_channels：指定期望的通道数，若为0则不做颜色空间变换
    *
    * 返回值：加载图像成功返回图像数据指针，否则返回NULL；
    */
    ImageData = stbi_load(FilePath.c_str(), &Size.X, &Size.Y, &ChannelSize, 0);
    MaxIndex = Size - 1;
    TextureType = InType;
}

UTexture::~UTexture()
{
    if (bCustomData)
    {
        free(CustomData);
    }
    else
    {
        stbi_image_free(ImageData);
    }
}

void UTexture::ClearCustomData() const
{
    if(bCustomData)
    {
        CustomData->Clear();
    }
}

void UTexture::SetColor(int X, int Y, const FVector4& Color)
{
    if (!bCustomData) return;

    CustomData->SetData(X, Y, Color);
}

FVector4 UTexture::GetColor(int X, int Y)
{
    FVector4 Ret;
    if (bCustomData)
    {
        ClampMaxIndex(X, Y);
        FVector4* Tmp = nullptr;
        CustomData->GetData(X, Y, Tmp);
        Ret = *Tmp;
        return Ret;
    }
    if (nullptr == ImageData) return Ret;

    unsigned char* DataPtr = ImageData + Y * Size.X * ChannelSize + X * ChannelSize;
    unsigned char R = *DataPtr++;
    unsigned char G = *DataPtr++;
    unsigned char B = *DataPtr++;
    unsigned char A = *DataPtr;
    Ret.X = R / 255.f;
    Ret.Y = G / 255.f;
    Ret.Z = B / 255.f;
    Ret.W = A / 255.f;

    if (TextureType == ETextureType::Normal)
    {
        //[0 - 1] -> [-1 - 1]
        Ret.X *= 2;
        Ret.Y *= 2;
        Ret.Z *= 2;
        Ret.W *= 2;

        Ret.X -= 1;
        Ret.Y -= 1;
        Ret.Z -= 1;
        Ret.W -= 1;
    }
    else if (TextureType == ETextureType::LinearColor)
    {
    }
    else if (TextureType == ETextureType::sRGB)
    {
        Ret.X = (float)pow(Ret.X, 2.2f);
        Ret.Y = (float)pow(Ret.Y, 2.2f);
        Ret.Z = (float)pow(Ret.Z, 2.2f);
    }

    return Ret;
}


FVector4 UTexture::SampleWithNearest(const FVector2& UV)
{
    float U = UV.X * Size.X - 0.5f;
    float V = UV.Y * Size.Y - 0.5f;
    int IntU = static_cast<int>(roundf(U));
    int IntV = static_cast<int>(roundf(V));

    ClampMaxIndex(IntU, IntV);

    return GetColor(IntU, IntV);
}

FVector4 UTexture::SampleWithBiLinear(const FVector2& UV)
{
    float U = UV.X * Size.X;
    float V = UV.Y * Size.Y;

    int U0 = static_cast<int>(U);
    int V0 = static_cast<int>(V);
    ClampMaxIndex(U0, V0);

    int U1 = U0 + 1;
    int V1 = V0;
    ClampMaxIndex(U1, V1);


    int U2 = U0;
    int V2 = V0 + 1;
    ClampMaxIndex(U2, V2);

    int U3 = U0 + 1;
    int V3 = V0 + 1;
    ClampMaxIndex(U3, V3);


    auto C0 = GetColor(U0, V0);
    auto C1 = GetColor(U1, V1);
    auto C2 = GetColor(U2, V2);
    auto C3 = GetColor(U3, V3);


    auto S1 = C0.Lerp(C1, U - U0);
    auto S2 = C2.Lerp(C3, U - U0);

    return S1.Lerp(S2, V - V0);
}

void UTexture::ClampMaxIndex(int& X, int& Y)
{
    X = (int)Clamp(X, 0, MaxIndex.X);
    Y = (int)Clamp(Y, 0, MaxIndex.Y);
}

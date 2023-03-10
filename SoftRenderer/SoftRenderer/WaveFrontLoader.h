#pragma once
#include <string>

#include "BaseStruct.h"

class UWaveFrontLoader
{
public:
    static FModelData* LoadFromFile(std::string FilePath);

    static void CalcTangentAndBiTangent(const FVector3& V1, const FVector3& V2, const FVector3& V3, const FVector2& UV1,
                                        const FVector2& UV2, const FVector2& UV3, const FVector3& Normal,
                                        FVector3& OutTangent, FVector3& OutBiTangent);
};

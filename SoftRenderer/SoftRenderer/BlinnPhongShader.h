#pragma once

#include "ShaderBase.h"

class UBlinnPhongShader : public UShaderBase
{
	FVector4 FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams, const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos, const FVector2& UV, const float Depth) const override;
};


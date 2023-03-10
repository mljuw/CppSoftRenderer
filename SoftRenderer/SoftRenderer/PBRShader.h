#pragma once
#include "ShaderBase.h"

struct FMatrix4x4;
struct FVector4;
struct FVector3;
struct FVector2;

class UPBRShader : public UShaderBase
{
public:
    FVSOutput VertexShader(const FVector3& Vertex, const FVector3& Normal, const FVector2& UV,
                           const FMatrix4x4& ModelMatrix, const FMatrix4x4& VPMatrix,
                           const FMatrix4x4& NormalToWorldMatrix) const override;

    FVector4 FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams,
                            const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos,
                            const FVector2& UV, const float Depth) const override;

    FVector4 BRDF(const FVector3& N, const FVector3& L, const FVector3& V, float Metallic, float Roughness,
                  const FVector4& BaseColor, const FVector4& LightColor, float Shiness) const;

    inline float G_SchlickBeckmenn(const FVector3& N, const FVector3& X, float K) const;

    inline float G_Smith(const FVector3& N, const FVector3& V, const FVector3& L, float K) const;

    inline float D_GGX(float AlphaPow, float NDotH) const;

    inline FVector3 F_Schlick(const FVector3& F0, float VDotH) const;
};

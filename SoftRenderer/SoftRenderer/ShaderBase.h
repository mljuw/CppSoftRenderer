#pragma once

struct FVSOutput;
struct FVector2;
struct FVector3;
struct FVector4;
struct FMatrix4x4;

class UShaderGlobal;
class UMaterialParams;


class UShaderBase
{
public:
    virtual FVSOutput VertexShader(const FVector3& Vertex, const FVector3& Normal, const FVector2& UV,
                                   const FMatrix4x4& ModelMatrix, const FMatrix4x4& VPMatrix,
                                   const FMatrix4x4& NormalToWorldMatrix) const;

    virtual FVector4 FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams,
                                    const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos,
                                    const FVector2& UV, const float Depth) const;
};

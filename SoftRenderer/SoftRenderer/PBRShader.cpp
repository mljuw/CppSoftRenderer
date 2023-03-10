#include "PBRShader.h"

#include "BaseStruct.h"
#include "Define.h"
#include "LightBase.h"

FVSOutput UPBRShader::VertexShader(const FVector3& Vertex, const FVector3& Normal, const FVector2& UV,
                                   const FMatrix4x4& ModelMatrix, const FMatrix4x4& VPMatrix,
                                   const FMatrix4x4& NormalToWorldMatrix) const
{
    return UShaderBase::VertexShader(Vertex, Normal, UV, ModelMatrix, VPMatrix, NormalToWorldMatrix);
}

FVector4 UPBRShader::FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams,
                                    const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos,
                                    const FVector2& UV,
                                    const float Depth) const
{
    // Lo(x, v) = Le(x, v) + Li(x, L) + f(x, L-V) * N dot L
    const FVector4 BaseColor = MatParams->PBRParams->SampleBaseColor(UV);
    const FVector4 Emissive = MatParams->PBRParams->SampleEmissive(UV);
    FVector3 TextureNormal = MatParams->SampleNormal(UV, Normal);
    TextureNormal = BTNMatrix.TransformVector(TextureNormal);
    TextureNormal.Normalize();

    float Metallic = 0.f;
    float Roughness = 1.f;
    float Shiness = MatParams->PBRParams->Shiness;
    
    const FVector4 AO = MatParams->PBRParams->SampleAO(UV);
    
    if(MatParams->PBRParams->IsCombinedTexture())
    {
        const FVector4 CombinedTex = MatParams->PBRParams->SampleCombinedTexture(UV);
        Metallic = CombinedTex.Y;
        Roughness = CombinedTex.X;
        Shiness = CombinedTex.Z;
    }
    else
    { 
        Metallic = MatParams->PBRParams->SampleMetallic(UV).X;
        Roughness = MatParams->PBRParams->SampleRoughness(UV).X;
    }
    
    
    FVector3 V = (ShaderGlobal->CameraTrans.GetLocation() - WorldPos);
    V.Normalize();

    FVector4 Lo;
    for (ULightBase* Light : ShaderGlobal->Lights)
    {
        FVector3 L = Light->GetLightDirection(WorldPos);
        const float NDotL = std::max(TextureNormal.DotProduct(L), 0.f);

        const float AttenuationFactor = pow(Light->GetAttenuationFactor(WorldPos), 2);

        FVector4 BRD = BRDF(TextureNormal, L, V,
                        Metallic, Roughness, BaseColor, Light->LightColor * AttenuationFactor, Shiness);

        Lo += (BRD * NDotL);
    }
    const FVector4 Ambient = ShaderGlobal->AmbientColor * AO.X * BaseColor;
    FVector4 Ret = Emissive + Ambient + Lo;
    Ret.W = 1;
    return Ret;
}

FVector4 UPBRShader::BRDF(const FVector3& N, const FVector3& L, const FVector3& V, float Metallic, float Roughness,
                          const FVector4& BaseColor, const FVector4& LightColor, float Shiness) const
{
    const FVector3 Diffuse = BaseColor / PI;
    
    FVector3 H = L + V;
    H.Normalize();
    
    static FVector3 PlasticsMetallic = FVector3(0.04f, 0.04f, 0.04f);
    const float Alpha = Roughness * Roughness;
    const float AlphaPow = Alpha * Alpha;
    const float K = Alpha / 2.f;
    const float NDotH = std::max(N.DotProduct(H), 0.f);
    const float VDotH = std::max(V.DotProduct(H), 0.f);
    const float NDotV = std::max(N.DotProduct(V), 0.f);
    const float NDotL = std::max(N.DotProduct(L), 0.f);
    const FVector3 F0 = PlasticsMetallic.Lerp(BaseColor, Metallic);
    
    const float D = D_GGX(AlphaPow, NDotH);
    const float G = G_Smith(N, V, L, K);
    const FVector3 F = F_Schlick(F0, VDotH);
    
    static FVector3 Unit3 = {1, 1, 1};
    const FVector3 Kd = (Unit3 - F) * (1 - Metallic);
    
    FVector3 Specular = (F * D * G) / std::max(4 * NDotV * NDotL, 0.0001f);
    Specular *= (LightColor * pow(Shiness, 2));
    
    return Kd * Diffuse + Specular;
}

float UPBRShader::G_Smith(const FVector3& N, const FVector3& V, const FVector3& L, float K) const
{
    return G_SchlickBeckmenn(N, L, K) * G_SchlickBeckmenn(N, V, K);
}

float UPBRShader::G_SchlickBeckmenn(const FVector3& N, const FVector3& X, float K) const
{
    const float NDotX = std::max(N.DotProduct(X), 0.f);
    return NDotX / (NDotX * (1.f - K) + K);
}

float UPBRShader::D_GGX(float AlphaPow, float NDotH) const
{
    return AlphaPow / (PI * powf(NDotH * NDotH  * (AlphaPow - 1.f) + 1, 2));
}

FVector3 UPBRShader::F_Schlick(const FVector3& F0, float VDotH) const
{
    static FVector3 Unit = {1, 1, 1};
    return F0 + (Unit - F0) * powf(1.f - VDotH, 5);
}

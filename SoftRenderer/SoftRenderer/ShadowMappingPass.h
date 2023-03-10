#pragma once

#include "Renderer.h"
#include "RenderPassBase.h"

class ULightBase;

class UShadowMappingPass : public URenderPassBase
{
protected:
    FMatrix4x4 CurVPMatrix;
    ULightBase* DirLight = nullptr;
    UTexture* ShadowMap = nullptr;
    UMyBuffer<float>* DepthBuffer = nullptr;

    std::vector<FSize> PCFOffset = {
        {-1, -1},
        {0, -1},
        {1, -1},
        {-1, 0},
        {0, 0},
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}
    };

public:
    UShadowMappingPass(URenderer* InRenderer);

    ~UShadowMappingPass() override;

    const FMatrix4x4& GetViewProjectionMatrix() const override;
    void PrePipeline() override;
    void BeginPipeline(FRenderModelProxy* Model) override;
    void PostPipeline() override;

    bool CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                             const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                             const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth) override;

    void FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                         const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                         const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent,
                         const FVector4& WorldBiTangent, float Depth, FVector4& OutColor) override;

    void Rasterization(FVSOutput (& Vertices)[3], FRenderModelProxy* Model, const FVector3& WorldTangent,
                       const FVector3& WorldBiTangent) override;

    void Clear() override;

    FVector4 SampleShadowMap(const FVector2& UV) const;

    void OnScreenSizeChange() override;

    float GetShadowAttenuation(const FVector4& WorldPos) const;

    void Init();
};

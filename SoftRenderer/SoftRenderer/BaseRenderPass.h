#pragma once
#include "RenderPassBase.h"


class UBaseRenderPass : public URenderPassBase
{
protected:
	FMatrix4x4 CurVPMatrix;

	FVector4 WireframeColor = FVector4(0.f, 1.f, 0.f, 1.f);

private:
	//TmpData
	float Xt;
	float FluoroscopicCorrectionParams[3];


public:
	UBaseRenderPass(URenderer* InRenderer) : URenderPassBase(InRenderer)
	{
	}

	void OnScreenSizeChange() override;
	void PrePipeline() override;
	void BeginPipeline(FRenderModelProxy* Model) override;
	void PostPipeline() override;

protected:
	const FMatrix4x4& GetViewProjectionMatrix() const override; 
	
	void Rasterization(FVSOutput (&Vertices)[3], FRenderModelProxy* Model,
		const FVector3& WorldTangent, const FVector3& WorldBiTangent) override;

	void FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
		const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent, const FVector4& WorldBiTangent, float Depth, FVector4& OutColor) override;

	FSize World2ScreenPos(const FVector4& WorldPos) const;

	bool CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
		const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth) override;
};
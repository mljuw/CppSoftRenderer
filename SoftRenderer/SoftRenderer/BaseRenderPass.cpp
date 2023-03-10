#include "BaseRenderPass.h"

#include <Windows.h>

#include "Define.h"
#include "Renderer.h"
#include "ShaderBase.h"
#include "ShadowMappingPass.h"


void UBaseRenderPass::OnScreenSizeChange()
{
	URenderPassBase::OnScreenSizeChange();
}

void UBaseRenderPass::PrePipeline()
{
	URenderPassBase::PrePipeline();
	if(Renderer->IsShowShadowMap())
	{
		return;
	}
	CurVPMatrix = CameraMatrix * ProjectionMatrix;
}

void UBaseRenderPass::BeginPipeline(FRenderModelProxy* Model)
{
	if(Renderer->IsShowShadowMap()) return;
	
	URenderPassBase::BeginPipeline(Model);
}

void UBaseRenderPass::PostPipeline()
{
	if(Renderer->IsShowShadowMap()) return;
	URenderPassBase::PostPipeline();
}

FSize UBaseRenderPass::World2ScreenPos(const FVector4& WorldPos) const
{
	FVector4 CVVPoint = WorldPos * CurVPMatrix;
	CVVPoint /= CVVPoint.W;
	const FVector4 ScreenPoint = CVVPoint * NDC2ScreenMatrix;
	return FSize(ScreenPoint.Y, ScreenPoint.Z);
}

void UBaseRenderPass::Rasterization(FVSOutput (&Vertices)[3],
	FRenderModelProxy* Model, const FVector3& WorldTangent, const FVector3& WorldBiTangent)
{
	for (FVSOutput& Vertex : Vertices)
	{

		Vertex.Pos = Vertex.Pos * NDC2ScreenMatrix;
		
		if (Renderer->IsDrawNormal())
		{
			FVector4 EndPoint = Vertex.WorldPos + Vertex.WorldNormal * Renderer->GetDrawNormalSize();
			EndPoint.W = 1;
			Renderer->DrawLine(FSize(Vertex.Pos.Y, Vertex.Pos.Z), World2ScreenPos(EndPoint),
				{ 0.f, 1.f, 0.f, 1.0f });
		}

		if (Renderer->IsDrawTangent())
		{
			if(!WorldTangent.IsNan())
			{
				FVector4 EndPoint = Vertex.WorldPos + WorldTangent * Renderer->GetDrawNormalSize();
				EndPoint.W = 1;
				Renderer->DrawLine(FSize(Vertex.Pos.Y, Vertex.Pos.Z), World2ScreenPos(EndPoint),
					{ 1.f, 0.f, 0.f, 1.0f });
			}
		}

		if (Renderer->IsDrawBiTangent())
		{
			if(!WorldBiTangent.IsNan())
			{
				FVector4 EndPoint = Vertex.WorldPos + WorldBiTangent * Renderer->GetDrawNormalSize();
				EndPoint.W = 1;
				Renderer->DrawLine(FSize(Vertex.Pos.Y, Vertex.Pos.Z), World2ScreenPos(EndPoint),
					{ 0.f, 0.f, 1.f, 1.0f });
			}
		}
	}

	const FVSOutput& V1 = Vertices[0];
	const FVSOutput& V2 = Vertices[1];
	const FVSOutput& V3 = Vertices[2];
	if (Renderer->IsWireframeMode())
	{
		FSize Points[3];
		Points[0] = { static_cast<int>(V1.Pos.Y), static_cast<int>(V1.Pos.Z) };
		Points[1] = { static_cast<int>(V2.Pos.Y), static_cast<int>(V2.Pos.Z) };
		Points[2] = { static_cast<int>(V3.Pos.Y), static_cast<int>(V3.Pos.Z) };
		Renderer->DrawTriangle(Points, WireframeColor);
		return;
	}


	URenderPassBase::Rasterization(Vertices, Model, WorldTangent, WorldBiTangent);
}

bool UBaseRenderPass::CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
	const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth)
{
	CalcFluoroscopicCorrectionParams(AlphaBetaGamma, V1.Depth, V2.Depth, V3.Depth, Xt,
		FluoroscopicCorrectionParams);

	OutDepth = (FluoroscopicCorrectionParams[0] * V1.Depth + FluoroscopicCorrectionParams[1] * V2.Depth +
		FluoroscopicCorrectionParams[2] * V3.Depth) * Xt;

	float* ExistsDepth = nullptr;
	if (FrameBuffer->DepthBuffer->GetData(X, Y, ExistsDepth) && *ExistsDepth >= 0)
	{
		if (*ExistsDepth < OutDepth)
		{
			return false;
		}
	}

	FrameBuffer->DepthBuffer->SetData(X, Y, OutDepth);
	return true;
}

void UBaseRenderPass::FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2,
	const FVSOutput& V3, const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent, const FVector4& WorldBiTangent, float Depth, FVector4& OutColor)
{
	/*FVector4 PixelNormal = (V1.WorldNormal * FluoroscopicCorrectionParams[0] + V2.WorldNormal * FluoroscopicCorrectionParams[1] +
		V3.WorldNormal * FluoroscopicCorrectionParams[2]) * Xt;*/

	FVector4 PixelWSPos = (V1.WorldPos * FluoroscopicCorrectionParams[0] + V2.WorldPos * FluoroscopicCorrectionParams[1] +
		V3.WorldPos * FluoroscopicCorrectionParams[2]) * Xt;

	FVector2 PixelUV = (V1.UV * FluoroscopicCorrectionParams[0] + V2.UV * FluoroscopicCorrectionParams[1] +
		V3.UV * FluoroscopicCorrectionParams[2]) * Xt;


	FMatrix4x4 BTNMatrix;
	BTNMatrix.XPlane = WorldTangent;
	BTNMatrix.YPlane = WorldBiTangent;
	BTNMatrix.ZPlane = V1.WorldNormal;
	OutColor = Shader->FragmentShader(Renderer->GetShaderGlobal(), MatParams, BTNMatrix, V1.WorldNormal, PixelWSPos, PixelUV, Depth);
}

const FMatrix4x4& UBaseRenderPass::GetViewProjectionMatrix() const
{
	return CurVPMatrix;
}

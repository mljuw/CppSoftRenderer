#include "ShadowMappingPass.h"
#include "LightBase.h"
#include "ShaderBase.h"
#include "UTexture.h"


UShadowMappingPass::UShadowMappingPass(URenderer* InRenderer) :
    URenderPassBase(InRenderer)
{
    Init();
}

void UShadowMappingPass::Init()
{
    const FSize ScreenSize = Renderer->GetScreenSize();
    SetOrthoProjection(5, 1250, ScreenSize.X, 2);

    delete ShadowMap;
    delete DepthBuffer;

    ShadowMap = new UTexture(ScreenSize, {1.f, 1.f, 1.f, 1.f});
    DepthBuffer = new UMyBuffer<float>(ScreenSize, -1);
}

UShadowMappingPass::~UShadowMappingPass()
{
    delete ShadowMap;
    delete DepthBuffer;
}

void UShadowMappingPass::OnScreenSizeChange()
{
    URenderPassBase::OnScreenSizeChange();

    Init();
}

void UShadowMappingPass::Clear()
{
    URenderPassBase::Clear();
    
    DepthBuffer->Clear();
    ShadowMap->ClearCustomData();
    DirLight = nullptr;
}

void UShadowMappingPass::PrePipeline()
{

    for (ULightBase* Light : Renderer->GetShaderGlobal()->Lights)
    {
        if (Light->LightType == ELightType::Directional)
        {
            DirLight = Light;
            break;
        }
    }
    if(nullptr == DirLight) return;

    URenderPassBase::PrePipeline();
    CurVPMatrix = DirLight->Transform.GetMatrix().GetInverse() * ProjectionMatrix;
}

void UShadowMappingPass::BeginPipeline(FRenderModelProxy* Model)
{
    if(!DirLight) return;
    URenderPassBase::BeginPipeline(Model);
}

void UShadowMappingPass::PostPipeline()
{
    if(!DirLight) return;
    URenderPassBase::PostPipeline();
}

const FMatrix4x4& UShadowMappingPass::GetViewProjectionMatrix() const
{
    return CurVPMatrix;
}


void UShadowMappingPass::FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                                         const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                                         const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent,
                                         const FVector4& WorldBiTangent, float Depth, FVector4& OutColor)
{
    OutColor = {Depth, Depth, Depth, 1};
    ShadowMap->SetColor(X, Y, OutColor);
}


bool UShadowMappingPass::CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                                             const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                                             const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth)
{
    OutDepth = AlphaBetaGamma.X * V1.Depth + AlphaBetaGamma.Y * V2.Depth + AlphaBetaGamma.Z * V3.Depth;
    float* ExistsDepth = nullptr;
    if (DepthBuffer->GetData(X, Y, ExistsDepth))
    {
        if (*ExistsDepth < OutDepth)
        {
            return false;
        }
    }

    DepthBuffer->SetData(X, Y, OutDepth);
    return true;
}

FVector4 UShadowMappingPass::SampleShadowMap(const FVector2& UV) const
{
    if (nullptr == ShadowMap) return {0, 0, 0, 0};
    return ShadowMap->SampleWithNearest(UV);
}

void UShadowMappingPass::Rasterization(FVSOutput (& Vertices)[3], FRenderModelProxy* Model,
                                       const FVector3& WorldTangent,
                                       const FVector3& WorldBiTangent)
{
    for (FVSOutput& Vertex : Vertices)
    {
        Vertex.Pos = Vertex.Pos * NDC2ScreenMatrix;
    }
    const FVSOutput& V1 = Vertices[0];
    const FVSOutput& V2 = Vertices[1];
    const FVSOutput& V3 = Vertices[2];

    const FSize V1PixelPos = FSize(V1.Pos.Y, V1.Pos.Z);
    const FSize V2PixelPos = FSize(V2.Pos.Y, V2.Pos.Z);
    const FSize V3PixelPos = FSize(V3.Pos.Y, V3.Pos.Z);

    const FSize* Points[3] = {&V1PixelPos, &V2PixelPos, &V3PixelPos};
    FSize Max, Min;
    CalcTriangleAABB(Points, Min, Max);

    FVector4 Color;
    float Depth;
    UFrameBuffer* CurFrameBuffer = Renderer->GetFrameBuffer(0);
    for (int PixelY = Min.Y; PixelY < Max.Y; ++PixelY)
    {
        for (int PixelX = Min.X; PixelX < Max.X; ++PixelX)
        {
            const float X = PixelX + 0.5f;
            const float Y = PixelY + 0.5f;
            FVector3 AlphaBetaGamma;
            CalcBarycentricCoordinateParams(V1PixelPos, V2PixelPos, V3PixelPos, {X, Y}, AlphaBetaGamma);
            if (!CheckPixelInTriangle(AlphaBetaGamma)) continue;

            if (!CheckAndUpdateDepth(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, AlphaBetaGamma,
                                     PixelX, PixelY, Depth))
                continue;

            FragmentProcess(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, AlphaBetaGamma, PixelX, PixelY,
                            WorldTangent, WorldBiTangent, Depth, Color);
            if (Renderer->IsShowShadowMap())
            {
                CurFrameBuffer->ColorBuffer->SetData(X, Y, Color);
            }
        }
    }
}

float UShadowMappingPass::GetShadowAttenuation(const FVector4& WorldPos) const
{
    if (nullptr == ShadowMap || nullptr == DirLight) return 1;
    const FVector4 NDCPos = WorldPos * CurVPMatrix;
    // const FVector2 UV = {NDCPos.Y * 0.5f + 0.5f, NDCPos.Z * -0.5f + 0.5f};
    // const FVector4& Depth = ShadowMap->SampleWithNearest(UV);
    
    const FVector4 ScreenPos = NDCPos * NDC2ScreenMatrix;
    const float LookDepth = NDCPos.X - 0.003f;
    int Counter = 0;
    for(const FSize& Offset : PCFOffset)
    {
        const FVector4& Depth = ShadowMap->GetColor(ScreenPos.Y + Offset.X, ScreenPos.Z + Offset.Y);
        if (LookDepth > Depth.X)
        {
            Counter++;
        }
    }
    return 1.f - (static_cast<float>(Counter) / PCFOffset.size());
}

#pragma once
#include "BaseStruct.h"

class URenderer;
class UFrameBuffer;
struct FVSOutput;
class UShaderBase;

enum class EProjectionType : unsigned char
{
    Perspective,
    Ortho,
};

enum class EClipPlaneType : unsigned char
{
    W,
    Near,
    Far,
    Top,
    Bottom,
    Left,
    Right,
};

struct MSAACulcInfo
{
    int FrameBufferIndex;
    FVector3 AlphaBetaGamma;
    float X;
    float Y;
};

class URenderPassBase
{
protected:
    FMatrix4x4 ProjectionMatrix;
    FMatrix4x4 NDC2ScreenMatrix;
    FMatrix4x4 CameraMatrix;
    URenderer* Renderer;

    std::vector<FVector2>* CurrentAAPixelOffset = nullptr;
    EProjectionType ProjectionType = EProjectionType::Perspective;

    float MaxDepth;

    std::vector<MSAACulcInfo> EnterMSAAInfo;

private:
    std::vector<FVector2> NoneAAPixelOffset =
    {
        {0.5f, 0.5f}
    };

    std::vector<FVector2> AAPixelOffset =
    {
        {0.25f, 0.25f},
        {0.75f, 0.25f},
        {0.25f, 0.75f},
        {0.75f, 0.75f}
    };

public:
    URenderPassBase(URenderer* InRenderer)
    {
        Renderer = InRenderer;
        CalcNDC2ScreenMatrix();
    }

    virtual ~URenderPassBase() = default;

    virtual void SetPerspectiveProjection(float Fov, float Near, float Far, float Ratio);

    virtual void SetOrthoProjection(float Near, float Far, float Width, float Ratio);

    virtual void OnScreenSizeChange();

    virtual void PrePipeline();

    virtual void BeginPipeline(FRenderModelProxy* Model);

    virtual void PostPipeline();

    virtual void Clear(){};
protected:

    virtual void Rasterization(FVSOutput (&Vertices)[3], FRenderModelProxy* Model,
                               const FVector3& WorldTangent, const FVector3& WorldBiTangent);

    virtual bool CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                                     const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                                     const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth);
    
    virtual void FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams,
                                 const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
                                 const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent,
                                 const FVector4& WorldBiTangent, float Depth, FVector4& OutColor);

    virtual const FMatrix4x4& GetViewProjectionMatrix() const = 0;

    bool IsBackFace(const FVector4& V1, const FVector4& V2, const FVector4& V3) const;

    void CalcBarycentricCoordinateParams(const FSize& V1, const FSize& V2, const FSize& V3, const FVector2& G,
                                         FVector3& OutParams) const;

    bool CheckPixelInTriangle(const FVector3& AlphaBetaGamma) const;

    void CalcNDC2ScreenMatrix();

    void CalcTriangleAABB(const FSize* (&Points)[3], FSize& OutMin, FSize& OutMax) const;

    void CalcFluoroscopicCorrectionParams(const FVector3& AlphaBetaGamma, float V1Depth, float V2Depth,
                                          float V3Depth, float& OutXt, float (&OutParams)[3]) const;

    bool CheckPointInClipPlane(EClipPlaneType PlaneType, const FVector4& CVVPoint) const;

    void ClipTriangleWithPlaneType(EClipPlaneType PlaneType, std::vector<FVSOutput>& Vertices) const;
};

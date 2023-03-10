#include "Renderer.h"

#include "BaseRenderPass.h"
#include "ShadowMappingPass.h"
#include "BlinnPhongShader.h"
#include "PBRShader.h"
#include "DataAssetLoader.h"


URenderer::URenderer(UDataAssetLoader* InAssetLoader)
{
    AssetLoader = InAssetLoader;
    
    ShadowPass = new UShadowMappingPass(this);
    RenderPassArray.push_back(ShadowPass);

    BasePass = new UBaseRenderPass(this);
    BasePass->SetPerspectiveProjection(90.f, 1, 1250, 2);
    RenderPassArray.push_back(BasePass);

    ShaderGlobal = new UShaderGlobal();
    ShaderGlobal->ShadowMapping = ShadowPass;
}

URenderer::~URenderer()
{
    delete ColorBuffer;
    delete ShaderGlobal;
    delete ShadowPass;
}


void URenderer::SetCameraLoc(const FVector3& Loc)
{
    ShaderGlobal->CameraTrans.SetLocation(Loc);
}

void URenderer::SetCameraRot(const FVector3& Rot)
{
    ShaderGlobal->CameraTrans.SetRotation(Rot);
}

void URenderer::SetScreenSize(int Width, int Height)
{
    ScreenSize.X = Width;
    ScreenSize.Y = Height;
    InitFrameBuffer();

    for (auto RenderPass : RenderPassArray)
    {
        RenderPass->OnScreenSizeChange();
    }
}

void URenderer::DrawLine(FSize Start, FSize End, const FVector4& Color)
{
    if (0 >= FrameBufferArray.size()) return;

    if (Start.X > End.X)
    {
        FSize Tmp = Start;
        Start = End;
        End = Tmp;
    }

    FSize Diff = End - Start;
    int YSign = 1;
    if (Diff.Y < 0)
    {
        YSign = -1;
    }

    Diff.X = abs(Diff.X);
    Diff.Y = abs(Diff.Y);

    bool bSlopeMoreThenOne = false;
    if (Diff.Y > Diff.X)
    {
        bSlopeMoreThenOne = true;
        const int TmpX = Diff.X;
        Diff.X = Diff.Y;
        Diff.Y = TmpX;
    }
    FSize CurPos = Start;

    const int DY = 2 * Diff.Y;
    const int DX = 2 * Diff.X;

    int N = 0;
    for (int i = 0; i < Diff.X; ++i)
    {
        GetFrameBuffer(0)->ColorBuffer->SetData(CurPos.X, CurPos.Y, Color);
        N = N + DY;
        if (N > Diff.X)
        {
            N = N - DX;
            if (!bSlopeMoreThenOne)
                CurPos.Y += YSign;
            else
                CurPos.X++;
        }

        if (!bSlopeMoreThenOne)
            CurPos.X++;
        else
            CurPos.Y += YSign;
    }
}

void URenderer::DrawTriangle(FSize (&Points)[3], const FVector4& Color)
{
    DrawLine(Points[0], Points[1], Color);
    DrawLine(Points[1], Points[2], Color);
    DrawLine(Points[2], Points[0], Color);
}

void URenderer::InitFrameBuffer()
{
    if (0 < FrameBufferArray.size())
    {
        for (UFrameBuffer* FrameBuffer : FrameBufferArray)
        {
            delete FrameBuffer;
        }
        FrameBufferArray.clear();
    }
    if (AntiAliasingType == EAntiAliasingType::None)
    {
        FrameBufferArray.push_back(new UFrameBuffer(ScreenSize));
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            FrameBufferArray.push_back(new UFrameBuffer(ScreenSize));
        }
    }

    if (ColorBuffer)
    {
        delete ColorBuffer;
    }
    ColorBuffer = new UMyBuffer<FVector4>(ScreenSize, {0, 0, 0, 1});
}

void URenderer::Clear()
{
    for (UFrameBuffer* FrameBuffer : FrameBufferArray)
    {
        FrameBuffer->ClearBuffer();
    }
    for (auto RenderPass : RenderPassArray)
    {
        RenderPass->Clear();
    }
    ColorBuffer->Clear();
}

void URenderer::ClearScene()
{
    ShaderGlobal->CameraTrans.SetLocation({0, 0, 0});
    ShaderGlobal->CameraTrans.SetRotation({0, 0, 0});
    RenderModels.clear();
    ShaderGlobal->Lights.clear();
}

void URenderer::Display(void* CanvasBuffer)
{
    if (FrameBufferArray.size() <= 0) return;

    for (auto RenderPass : RenderPassArray)
    {
        RenderPass->PrePipeline();
    }

    for (auto RenderPass : RenderPassArray)
    {
        for (auto Model : RenderModels)
        {
            RenderPass->BeginPipeline(Model);
        }
    }

    for (auto RenderPass : RenderPassArray)
    {
        RenderPass->PostPipeline();
    }

    const int RowSize = ScreenSize.X * 4;

    FVector4* Color = nullptr;
    FVector4* ExistsColor = nullptr;

    const unsigned int* ValidDataIdxArray;
    unsigned int ValidDataArraySize;

    for (const UFrameBuffer* FrameBuffer : FrameBufferArray)
    {
        FrameBuffer->ColorBuffer->GetValidDataIdxArray(ValidDataIdxArray, ValidDataArraySize);
        for (int i = 0; i < ValidDataArraySize; ++i)
        {
            unsigned int Idx = ValidDataIdxArray[i];
            FrameBuffer->ColorBuffer->GetDataByIndex(Idx, Color);
            Color->Clamp(0, 1);

            if (ColorBuffer->GetDataByIndex(Idx, ExistsColor))
            {
                *ExistsColor += *Color;
            }
            else
            {
                ColorBuffer->SetDataByIndex(Idx, *Color);
            }
        }
    }


    ColorBuffer->GetValidDataIdxArray(ValidDataIdxArray, ValidDataArraySize);
    for (int i = 0; i < ValidDataArraySize; ++i)
    {
        unsigned int Idx = ValidDataIdxArray[i];
        ColorBuffer->GetDataByIndex(Idx, Color);

        *Color /= FrameBufferArray.size();

        const int Row = Idx / ScreenSize.X;
        const int Col = Idx % ScreenSize.X;

        unsigned char* Rgba = static_cast<unsigned char*>(CanvasBuffer);
        const int CanvasIndex = (ScreenSize.Y - 1 - Row) * RowSize + Col * 4;

        Rgba[CanvasIndex] = static_cast<unsigned char>(Color->Z * 255);
        Rgba[CanvasIndex + 1] = static_cast<unsigned char>(Color->Y * 255);
        Rgba[CanvasIndex + 2] = static_cast<unsigned char>(Color->X * 255);
        Rgba[CanvasIndex + 3] = static_cast<unsigned char>(Color->W * 255);

        /*Rgba[CanvasIndex] = static_cast<unsigned char>(Test.Z * 255);
        Rgba[CanvasIndex + 1] = static_cast<unsigned char>(Test.Y * 255);
        Rgba[CanvasIndex + 2] = static_cast<unsigned char>(Test.X * 255);
        Rgba[CanvasIndex + 3] = static_cast<unsigned char>(Test.W * 255);*/
    }
}


FRenderModelProxy* URenderer::AddModel(std::string FilePath, EShaderType ShaderType, const FTransform& Transform,
                                       UMaterialParams* MaterialParam)
{
    FModelData* ModelData = AssetLoader->LoadModel(FilePath);
    FRenderModelProxy* Proxy = new FRenderModelProxy;
    Proxy->ModelData = ModelData;
    Proxy->Transform = Transform;
    Proxy->MatParams = MaterialParam;

    if (ShaderType == EShaderType::BlinnPhong)
    {
        Proxy->Shader = new UBlinnPhongShader();
    }
    else if (ShaderType == EShaderType::PBR)
    {
        Proxy->Shader = new UPBRShader();
    }

    RenderModels.push_back(Proxy);
    return Proxy;
}

UFrameBuffer* URenderer::GetFrameBuffer(unsigned BufferIndex)
{
    return FrameBufferArray[BufferIndex];
}

void URenderer::SetAAType(EAntiAliasingType NewType)
{
    if (AntiAliasingType != NewType)
    {
        AntiAliasingType = NewType;
        InitFrameBuffer();
    }
}

void URenderer::AddLight(ULightBase* Light)
{
    ShaderGlobal->Lights.push_back(Light);
}


void URenderer::SetAmbientColor(const FVector4& InColor)
{
    ShaderGlobal->AmbientColor = InColor;
}

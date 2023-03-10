#pragma once
#include <string>

#include "BaseStruct.h"
#include "Define.h"
#include "FrameBuffer.h"

class URenderPassBase;
class UBaseRenderPass;
class UShadowMappingPass;
class ULightBase;
class UDataAssetLoader;

class URenderer
{
protected:
    std::vector<UFrameBuffer*> FrameBufferArray;

    FSize ScreenSize;

    std::vector<URenderPassBase*> RenderPassArray;

    UBaseRenderPass* BasePass = nullptr;
    UShadowMappingPass* ShadowPass = nullptr;

    std::vector<FRenderModelProxy*> RenderModels;

    bool bWireframeMode = false;

    EAntiAliasingType AntiAliasingType = EAntiAliasingType::None;

    UMyBuffer<FVector4>* ColorBuffer = nullptr;

    bool bDrawNormal = false;
    bool bDrawTangent = false;
    bool bDrawBiTangent = false;
    bool bShowShadowMap = false;
    float DrawNormalSize = 5.f;

    UShaderGlobal* ShaderGlobal = nullptr;
    UDataAssetLoader* AssetLoader = nullptr;

public:
    URenderer(UDataAssetLoader* AssetLoader);

    ~URenderer();

    void ClearScene();

    void SetCameraLoc(const FVector3& Loc);

    void SetCameraRot(const FVector3& Rot);

    void SetScreenSize(int Width, int Height);

    const FSize& GetScreenSize() const { return ScreenSize; }

    void DrawLine(FSize InStart, FSize InEnd, const FVector4& Color);

    void DrawTriangle(FSize (&Points)[3], const FVector4& Color);

    void Clear();

    void Display(void* CanvasBuffer);

    FRenderModelProxy* AddModel(std::string FilePath, EShaderType ShaderType, const FTransform& Transform,
                                UMaterialParams* MaterialParam);

    const FTransform& GetCameraTransform() const { return ShaderGlobal->CameraTrans; }

    void EnableWireframeMode(bool InValue) { bWireframeMode = InValue; }

    bool IsWireframeMode() const { return bWireframeMode; }

    UFrameBuffer* GetFrameBuffer(unsigned int BufferIndex);

    EAntiAliasingType GetAAType() const { return AntiAliasingType; }

    void SetAAType(EAntiAliasingType NewType);

    bool IsDrawNormal() const { return bDrawNormal; }
    bool IsDrawTangent() const { return bDrawTangent; }
    bool IsDrawBiTangent() const { return bDrawBiTangent; }
    float GetDrawNormalSize() const { return DrawNormalSize; }
    bool IsShowShadowMap() const { return bShowShadowMap; }

    void EnableDrawNormal(bool bValue) { bDrawNormal = bValue; }
    void EnableDrawTangent(bool bValue) { bDrawTangent = bValue; }
    void EnableDrawBiTangent(bool bValue) { bDrawBiTangent = bValue; }

    void EnableShowShadowMap(bool bValue) { bShowShadowMap = bValue; }

    void AddLight(ULightBase* Light);

    void SetAmbientColor(const FVector4& InColor);

    UShaderGlobal* GetShaderGlobal() const { return ShaderGlobal; }

    UShadowMappingPass* GetShadowPass() const { return ShadowPass; }

protected:
    void InitFrameBuffer();
};

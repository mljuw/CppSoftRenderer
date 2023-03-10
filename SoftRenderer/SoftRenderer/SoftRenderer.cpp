// SoftRenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <chrono>
#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>

#include "DataAssetLoader.h"
#include "Renderer.h"
#include "UTexture.h"
#include "WaveFrontLoader.h"
#include "DirectionalLight.h"
#include "PointLight.h"

using namespace std::chrono;

bool bEnableMouseDrop = false;
FSize MousePos;
FSize MouseMoveDiff;


void ShowScene(ESceneType SceneType, void* InBuffer, int InWidth, int InHeight);

bool bChangeScene = false;
bool bShowNormal = false;
bool bWireframeMode = false;
bool bShadowMapMode = false;
EAntiAliasingType AAType = EAntiAliasingType::None;
HMENU hOptionMenuPop;
HMENU hAAMenuPop;
void* buffer = nullptr;
int width;
int height;
URenderer* G_Renderer = nullptr;
ESceneType G_CurSceneType = ESceneType::Wireframe;
UDataAssetLoader* AssetLoader = nullptr;
 
#define IDM_SCENE_WIREFRAME  40001
#define IDM_SCENE_BLINNPHONG 40002
#define IDM_SCENE_PBR 40003
#define IDM_SCENE_SHADOW 40004


#define IDM_SCENE_WIREFRAME_MODE 50001
#define IDM_SCENE_SHOWNORMAL 50002
#define IDM_SCENE_SHOWSHADOWMAP 50003

#define IDM_SCENE_NONEAA 60001
#define IDM_SCENE_SSAA 60002
#define IDM_SCENE_MSAA 60003


void OnClickMenuOption(HWND hWnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDM_SCENE_WIREFRAME:
        ShowScene(ESceneType::Wireframe, buffer, width, height);
        break;
    case IDM_SCENE_BLINNPHONG:
        ShowScene(ESceneType::BlinnPhong, buffer, width, height);
        break;
    case IDM_SCENE_PBR:
        ShowScene(ESceneType::PBR, buffer, width, height);
        break;
    case IDM_SCENE_SHADOW:
        ShowScene(ESceneType::Shadow, buffer, width, height);
        break;
    case IDM_SCENE_WIREFRAME_MODE:
        bWireframeMode = !bWireframeMode;
        CheckMenuItem(hOptionMenuPop, IDM_SCENE_WIREFRAME_MODE, bWireframeMode ? MF_CHECKED : MF_UNCHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    case IDM_SCENE_SHOWNORMAL:
        bShowNormal = !bShowNormal;
        CheckMenuItem(hOptionMenuPop, IDM_SCENE_SHOWNORMAL, bShowNormal ? MF_CHECKED : MF_UNCHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    case IDM_SCENE_SHOWSHADOWMAP:
        bShadowMapMode = !bShadowMapMode;
        CheckMenuItem(hOptionMenuPop, IDM_SCENE_SHOWSHADOWMAP, bShadowMapMode ? MF_CHECKED : MF_UNCHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    case IDM_SCENE_NONEAA:
        AAType = EAntiAliasingType::None;
        CheckMenuItem(hAAMenuPop, IDM_SCENE_NONEAA, MF_CHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_SSAA, MF_UNCHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_MSAA, MF_UNCHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    case IDM_SCENE_SSAA:
        AAType = EAntiAliasingType::SSAA;
        CheckMenuItem(hAAMenuPop, IDM_SCENE_NONEAA, MF_UNCHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_SSAA, MF_CHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_MSAA, MF_UNCHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    case IDM_SCENE_MSAA:
        AAType = EAntiAliasingType::MSAA;
        CheckMenuItem(hAAMenuPop, IDM_SCENE_NONEAA, MF_UNCHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_SSAA, MF_UNCHECKED);
        CheckMenuItem(hAAMenuPop, IDM_SCENE_MSAA, MF_CHECKED);
        ShowScene(G_CurSceneType, buffer, width, height);
        break;
    default:
        break;
    }
}


LRESULT CALLBACK winProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int X, Y;
    switch (msg)
    {
    case WM_SIZE:
        break;
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        bEnableMouseDrop = true;
        MousePos.X = GET_X_LPARAM(lParam);
        MousePos.Y = GET_Y_LPARAM(lParam);
        break;
    case WM_MOUSEMOVE:
        X = GET_X_LPARAM(lParam);
        Y = GET_Y_LPARAM(lParam);
        MouseMoveDiff.X = X - MousePos.X;
        MouseMoveDiff.Y = Y - MousePos.Y;

        MousePos.X = GET_X_LPARAM(lParam);
        MousePos.Y = GET_Y_LPARAM(lParam);
        break;
    case WM_LBUTTONUP:
        MouseMoveDiff = {0, 0};
        bEnableMouseDrop = false;
        break;
    case WM_COMMAND:
        OnClickMenuOption(hWnd, wParam);
        return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    AssetLoader = new UDataAssetLoader("\\..\\..\\..\\Models");
    G_Renderer = new URenderer(AssetLoader);
    
    //动态菜单  
    HMENU hMenu = CreateMenu(); //主菜单,水平  

    HMENU hSceneMenuPop = CreateMenu(); //下拉的菜单,垂直  
    AppendMenu(hSceneMenuPop,MF_STRING,IDM_SCENE_WIREFRAME,_T("线框模式"));
    AppendMenu(hSceneMenuPop,MF_STRING,IDM_SCENE_BLINNPHONG,_T("BlinnPhong"));
    AppendMenu(hSceneMenuPop,MF_STRING,IDM_SCENE_PBR,_T("PBR"));
    AppendMenu(hSceneMenuPop,MF_STRING,IDM_SCENE_SHADOW,_T("阴影"));
    AppendMenu(hMenu,MF_POPUP, (unsigned int)hSceneMenuPop,_T("场景"));
    //把下拉的菜单加载到主菜单第一个菜单,说的有点形象,  
    //自己运行程序看效果  

    hOptionMenuPop = CreateMenu();
    AppendMenu(hOptionMenuPop,MF_CHECKED, IDM_SCENE_WIREFRAME_MODE,_T("显示线框"));
    AppendMenu(hOptionMenuPop,MF_CHECKED,IDM_SCENE_SHOWNORMAL,_T("法线"));
    AppendMenu(hOptionMenuPop,MF_CHECKED,IDM_SCENE_SHOWSHADOWMAP,_T("阴影贴图（仅阴影场景有效）"));
    AppendMenu(hMenu,MF_POPUP, (unsigned int)hOptionMenuPop,_T("显示选项"));

    CheckMenuItem(hOptionMenuPop, IDM_SCENE_WIREFRAME_MODE, MF_UNCHECKED);
    CheckMenuItem(hOptionMenuPop, IDM_SCENE_SHOWNORMAL, MF_UNCHECKED);
    CheckMenuItem(hOptionMenuPop, IDM_SCENE_SHOWSHADOWMAP, MF_UNCHECKED);

    hAAMenuPop = CreateMenu();
    AppendMenu(hAAMenuPop,MF_CHECKED, IDM_SCENE_NONEAA,_T("关闭"));
    AppendMenu(hAAMenuPop,MF_CHECKED,IDM_SCENE_SSAA,_T("超级采样"));
    AppendMenu(hAAMenuPop,MF_CHECKED,IDM_SCENE_MSAA,_T("多重采样"));
    AppendMenu(hMenu,MF_POPUP, (unsigned int)hAAMenuPop,_T("抗锯齿方式"));

    CheckMenuItem(hAAMenuPop, IDM_SCENE_NONEAA, MF_CHECKED);
    CheckMenuItem(hAAMenuPop, IDM_SCENE_SSAA, MF_UNCHECKED);
    CheckMenuItem(hAAMenuPop, IDM_SCENE_MSAA, MF_UNCHECKED);


    ::WNDCLASSEXA WinClass;
    WinClass.lpszClassName = "SoftRenderer";
    WinClass.cbSize = sizeof(::WNDCLASSEX);
    WinClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    WinClass.lpfnWndProc = winProc;
    WinClass.hInstance = hInstance;
    WinClass.hIcon = 0;
    WinClass.hIconSm = 0;
    WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WinClass.hbrBackground = (HBRUSH)(BLACK_BRUSH);
    WinClass.lpszMenuName = NULL;
    WinClass.cbClsExtra = 0;
    WinClass.cbWndExtra = 0;

    RegisterClassExA(&WinClass);


    HWND hWnd = CreateWindowEx(NULL, _T("SoftRenderer"), _T(""),
                               WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                               0, 0, 1440, 720, 0, hMenu, hInstance, 0);

    UpdateWindow(hWnd);

    ShowWindow(hWnd, SW_SHOW);

    RECT rt = {0, 0, 0, 0};
    GetClientRect(hWnd, &rt);
    width = rt.right - rt.left;
    height = rt.bottom - rt.top;

    HDC hDC = GetDC(hWnd);

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biSizeImage = 0;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;

    HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&buffer, 0, 0);

    HDC hMem = ::CreateCompatibleDC(hDC);
    SelectObject(hMem, hBmp);
    memset(buffer, 0, width * height * 4);

    MSG msg = {0};

    milliseconds LastMS = std::chrono::duration_cast<milliseconds>(
        system_clock::now().time_since_epoch());


    G_Renderer->SetScreenSize(width, height);

    ShowScene(ESceneType::Wireframe, buffer, width, height);
    while (true)
    {
        if (msg.message == WM_DESTROY || msg.message == WM_CLOSE || msg.message == WM_QUIT)
        {
            break;
        }
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        milliseconds CurMS = std::chrono::duration_cast<milliseconds>(
            system_clock::now().time_since_epoch());

        float DeltaSeconds = (CurMS - LastMS).count() / 1000.f;

        BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
        // Sleep(11.1f);
        LastMS = CurMS;
    }

    return 0;
}

void LoadPlaneModel(URenderer* Renderer)
{
    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {1.0f, 1.0f, 1.0f, 1.0f};

    MaterialParam->DiffuseTexture = AssetLoader->LoadTexture("//Drone//DroneRedMangaDiffuse.tga.png",
                                                             ETextureType::LinearColor);

    MaterialParam->SpecularTexture = AssetLoader->LoadTexture("//Drone//DroneRedMangaSpecular.tga.png",
                                                              ETextureType::LinearColor);
    MaterialParam->NormalTexture = AssetLoader->LoadTexture("//Drone//DroneRedMangaNormals.tga.png",
                                                            ETextureType::Normal);


    FTransform Transform;
    Transform.SetLocation({250, -20, -40});
    Transform.SetRotation({0.f, 0.f, -10.f});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//Drone//model_1.obj",
                                                             EShaderType::BlinnPhong, Transform, MaterialParam);
}

void LoadGroundModel(URenderer* Renderer)
{
    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {1.f, 1.f, 1.f, 1.f};


    FTransform Transform;
    Transform.SetLocation({-150, -400, -200});
    Transform.SetRotation({0.f, -90.f, 0.f});
    Transform.SetScale3D({8, 8, 8});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//PlaneMesh.OBJ",
                                                             EShaderType::BlinnPhong, Transform, MaterialParam);
}

void LoadPBRModel1(URenderer* Renderer)
{
    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {1.0f, 1.0f, 1.0f, 1.0f};
    MaterialParam->NormalTexture = AssetLoader->LoadTexture("//PBR//Model1//T_Weapon_Set1_Normal.PNG",
                                                            ETextureType::Normal);

    MaterialParam->PBRParams->Emissive = AssetLoader->LoadTexture("//PBR//Model1//T_Emissive.PNG",
                                                                  ETextureType::sRGB);
    MaterialParam->PBRParams->Metallic = AssetLoader->LoadTexture("//PBR//Model1//T_Metalic.PNG",
                                                                  ETextureType::LinearColor);
    MaterialParam->PBRParams->Roughness = AssetLoader->LoadTexture("//PBR//Model1//T_Roughness.PNG",
                                                                   ETextureType::LinearColor);
    MaterialParam->PBRParams->AO = AssetLoader->LoadTexture("//PBR//Model1//T_AO.PNG",
                                                            ETextureType::LinearColor);
    MaterialParam->PBRParams->BaseColor = AssetLoader->LoadTexture("//PBR//Model1//T_BaseColor.PNG",
                                                                   ETextureType::sRGB);


    FTransform Transform;
    Transform.SetLocation({22, -10, -10});
    Transform.SetRotation({0.f, 90.f, 0.f});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//PBR//Model1//SM_Weapon.OBJ",
                                                             EShaderType::PBR, Transform, MaterialParam);
}

void LoadPBRModel2(URenderer* Renderer)
{
    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {1.0f, 1.0f, 1.0f, 1.0f};
    MaterialParam->NormalTexture = AssetLoader->LoadTexture("//PBR//Model2//T_Hero_weapon_27_N.PNG",
                                                            ETextureType::Normal);

    MaterialParam->PBRParams->BaseColor = AssetLoader->LoadTexture("//PBR//Model2//T_Hero_weapon_27_D.PNG",
                                                                   ETextureType::sRGB);

    MaterialParam->PBRParams->RMSCombineTexture = AssetLoader->LoadTexture("//PBR//Model2//T_Hero_weapon_27_S.PNG",
                                                                           ETextureType::LinearColor);


    FTransform Transform;
    Transform.SetLocation({22, -15, 10});
    Transform.SetRotation({0.f, 90.f, 0.f});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//PBR//Model2//SM_Hero27_Weapon2.OBJ",
                                                             EShaderType::PBR, Transform, MaterialParam);
}

void LoadPBRModel3(URenderer* Renderer)
{
    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {1.0f, 1.0f, 1.0f, 1.0f};
    MaterialParam->NormalTexture = AssetLoader->LoadTexture("//PBR//Model3//T_Hero_5_Weapon_N.PNG",
                                                            ETextureType::Normal);

    MaterialParam->PBRParams->BaseColor = AssetLoader->LoadTexture("//PBR//Model3//T_Hero_5_Weapon_D.PNG",
                                                                   ETextureType::sRGB);
    MaterialParam->PBRParams->Emissive = AssetLoader->LoadTexture("//PBR//Model3//T_Hero_5_Weapon_E.PNG",
                                                                  ETextureType::sRGB);


    MaterialParam->PBRParams->RMSCombineTexture = AssetLoader->LoadTexture("//PBR//Model3//T_Hero_5_Weapon_S.PNG",
                                                                           ETextureType::LinearColor);


    FTransform Transform;
    Transform.SetLocation({22, -3, -10});
    Transform.SetRotation({0.f, 90.f, 0.f});
    Transform.SetScale3D({0.45f, 0.45f, 0.45f});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//PBR//Model3//SM_Hero11_Weapon.OBJ",
                                                             EShaderType::PBR, Transform, MaterialParam);
}

void LoadWireFrame(URenderer* Renderer, void* buffer, int width, int height)
{
    Renderer->EnableWireframeMode(true);
    Renderer->SetCameraLoc({-10, 0, 0});

    UDirectionalLight* DirLight = new UDirectionalLight;
    DirLight->Transform.SetLocation({-100, 0, 100});
    DirLight->Transform.SetRotation({0.f, -30.f, 0.f});

    Renderer->AddLight(DirLight);


    UMaterialParams* MaterialParam = new UMaterialParams;
    MaterialParam->AmbientAbsorbance = {0.1f, 0.1f, 0.1f, 0.1f};
    MaterialParam->DiffuseTexture = AssetLoader->LoadTexture("//Sword//MyDiffColor.PNG", ETextureType::LinearColor);
    MaterialParam->SpecularTexture = AssetLoader->LoadTexture("//Sword//MySpecColor.PNG", ETextureType::LinearColor);
    MaterialParam->NormalTexture = AssetLoader->LoadTexture("//Sword//Sword_NM.PNG", ETextureType::Normal);


    FTransform Transform;
    Transform.SetLocation({50, -20, -40});
    FRenderModelProxy* RenderModelProxy = Renderer->AddModel("//Sword//Sword.OBJ",
                                                             EShaderType::BlinnPhong, Transform, MaterialParam);

    memset(buffer, 0, width * height * 4);

    Renderer->Clear();

    Renderer->Display(buffer);
}

void LoadBlinnPhongScene(URenderer* Renderer, void* buffer, int width, int height)
{
    Renderer->SetAmbientColor({0.5f, 0.5f, 0.5f, 1});

    Renderer->SetCameraLoc({0, 0, 150});
    Renderer->SetCameraRot({0, -45, 0});

    UDirectionalLight* DirLight = new UDirectionalLight;
    DirLight->Transform.SetLocation({100, 0, 100});
    DirLight->Transform.SetRotation({0.f, -90.f, 0.f});

    Renderer->AddLight(DirLight);


    LoadPlaneModel(Renderer);

    memset(buffer, 0, width * height * 4);

    Renderer->Clear();

    Renderer->Display(buffer);
}

void LoadShadowScene(URenderer* Renderer, void* buffer, int width, int height)
{
    Renderer->SetAmbientColor({0.5f, 0.5f, 0.5f, 1});

    Renderer->SetCameraLoc({0, 0, 150});
    Renderer->SetCameraRot({0, -45, 0});

    UDirectionalLight* DirLight = new UDirectionalLight;
    DirLight->Transform.SetLocation({100, 0, 100});
    DirLight->Transform.SetRotation({0.f, -90.f, 0.f});

    Renderer->AddLight(DirLight);


    LoadPlaneModel(Renderer);
    LoadGroundModel(Renderer);

    memset(buffer, 0, width * height * 4);

    Renderer->Clear();

    Renderer->Display(buffer);
}


void LoadPBRScene(URenderer* Renderer, void* buffer, int width, int height)
{
    Renderer->SetAmbientColor({0.35f, 0.35f, 0.35f, 1});

    Renderer->SetCameraLoc({0, 0, 0});
    Renderer->SetCameraRot({0, 0, 0});

    UPointLight* PointLight = new UPointLight;
    PointLight->SetSourceRadius(50);
    PointLight->SetAttenuationRadius(5000);
    PointLight->Transform.SetLocation({-350, -150, -50});
    Renderer->AddLight(PointLight);


    UPointLight* PointLight2 = new UPointLight;
    PointLight2->SetSourceRadius(50);
    PointLight2->SetAttenuationRadius(5000);
    PointLight2->Transform.SetLocation({-150, 150, 80});
    Renderer->AddLight(PointLight2);

    // LoadPBRModel1(Renderer);
    LoadPBRModel2(Renderer);
    LoadPBRModel3(Renderer);


    memset(buffer, 0, width * height * 4);
    Renderer->Clear();
    Renderer->Display(buffer);
}

void ShowScene(ESceneType SceneType, void* InBuffer, int InWidth, int InHeight)
{
    G_CurSceneType = SceneType;

    G_Renderer->ClearScene();
    G_Renderer->SetAAType(AAType);
    G_Renderer->EnableShowShadowMap(bShadowMapMode);
    G_Renderer->EnableDrawNormal(bShowNormal);
    G_Renderer->EnableDrawTangent(bShowNormal);
    G_Renderer->EnableDrawBiTangent(bShowNormal);
    G_Renderer->EnableWireframeMode(bWireframeMode);


    switch (SceneType)
    {
    case ESceneType::Shadow:
        LoadShadowScene(G_Renderer, InBuffer, InWidth, InHeight);
        break;
    case ESceneType::Wireframe:
        LoadWireFrame(G_Renderer, InBuffer, InWidth, InHeight);
        break;
    case ESceneType::BlinnPhong:
        LoadBlinnPhongScene(G_Renderer, InBuffer, InWidth, InHeight);
        break;
    case ESceneType::PBR:
        LoadPBRScene(G_Renderer, InBuffer, InWidth, InHeight);
        break;
    }
}

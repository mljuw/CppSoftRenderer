#pragma once

#define PI 3.141592654f
#define SMALL_NUMBER 0.0001f


static float Clamp(float Value, float Min, float Max)
{
    if (Value < Min)
        return Min;
    if (Value > Max)
        return Max;
    return Value;
}

static int Clamp(int Value, int Min, int Max)
{
    if (Value < Min)
        return Min;
    if (Value > Max)
        return Max;
    return Value;
}

enum class EAntiAliasingType : unsigned char
{
    None,
    SSAA,
    MSAA
};

enum class EShaderType : unsigned char
{
    BlinnPhong,
    PBR,
};

enum class ESceneType : unsigned char
{
    Wireframe,
    BlinnPhong,
    PBR,
    Shadow,
};

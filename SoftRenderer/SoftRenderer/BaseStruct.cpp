#include "BaseStruct.h"

#include "Define.h"
#include "UTexture.h"

FVector3::FVector3(const FVector4& Vect4)
{
    X = Vect4.X;
    Y = Vect4.Y;
    Z = Vect4.Z;
}

FVector4 FVector4::operator*(const FMatrix4x4& InMatrix) const
{
    FVector4 Ret;
    Ret.X = X * InMatrix.XPlane.X + Y * InMatrix.YPlane.X + Z * InMatrix.ZPlane.X + W * InMatrix.WPlane.X;
    Ret.Y = X * InMatrix.XPlane.Y + Y * InMatrix.YPlane.Y + Z * InMatrix.ZPlane.Y + W * InMatrix.WPlane.Y;
    Ret.Z = X * InMatrix.XPlane.Z + Y * InMatrix.YPlane.Z + Z * InMatrix.ZPlane.Z + W * InMatrix.WPlane.Z;
    Ret.W = X * InMatrix.XPlane.W + Y * InMatrix.YPlane.W + Z * InMatrix.ZPlane.W + W * InMatrix.WPlane.W;
    return Ret;
}


FVector4& FVector4::operator/=(float Value)
{
    X /= Value;
    Y /= Value;
    Z /= Value;
    W /= Value;
    return *this;
}

void FTransform::SetLocation(const FVector3& InLoc)
{
    Loc = InLoc;
    bDirty = true;
}

void FTransform::SetRotation(const FVector3& InRotation)
{
    Rot = InRotation;
    bDirty = true;
}

const FVector3& FTransform::GetRotation() const
{
    return Rot;
}

FVector3& FVector3::operator*=(const FVector4& Value)
{
    X *= Value.X;
    Y *= Value.Y;
    Z *= Value.Z;
    return *this;
}

void FTransform::SetScale3D(const FVector3& InScale)
{
    Scale3D = InScale;
    bDirty = true;
}

const FMatrix4x4& FTransform::GetMatrix() const
{
    if (bDirty)
    {
        bDirty = false;

        FMatrix4x4 ScaleMatrix;
        FMatrix4x4 MoveMatrix;

        ScaleMatrix.XPlane.X = Scale3D.X;
        ScaleMatrix.YPlane.Y = Scale3D.Y;
        ScaleMatrix.ZPlane.Z = Scale3D.Z;

        MoveMatrix.WPlane.X = Loc.X;
        MoveMatrix.WPlane.Y = Loc.Y;
        MoveMatrix.WPlane.Z = Loc.Z;

        FMatrix4x4 LinearMatrix = ScaleMatrix * GetRotationMatrix();
        Matrix = LinearMatrix * MoveMatrix;
    }

    return Matrix;
}

FMatrix4x4 FTransform::GetRotationMatrix() const
{
    FMatrix4x4 RollRotMatrix;
    FMatrix4x4 PitchRotMatrix;
    FMatrix4x4 YawRotMatrix;

    const float RollRadian = Rot.X * PI / 180.f;
    const float PitchRadian = Rot.Y * PI / 180.f;
    const float YawRadian = Rot.Z * PI / 180.f;

    const float RollCos = cos(RollRadian);
    const float RollSin = sin(RollRadian);

    const float PitchCos = cos(PitchRadian);
    const float PitchSin = sin(PitchRadian);

    const float YawCos = cos(YawRadian);
    const float YawSin = sin(YawRadian);

    RollRotMatrix.YPlane = {0, RollCos, RollSin, 0};
    RollRotMatrix.ZPlane = {0, -RollSin, RollCos, 0};

    PitchRotMatrix.XPlane = {PitchCos, 0, PitchSin, 0};
    PitchRotMatrix.ZPlane = {-PitchSin, 0, PitchCos, 0};

    YawRotMatrix.XPlane = {YawCos, -YawSin, 0, 0};
    YawRotMatrix.YPlane = {YawSin, YawCos, 0, 0};
    return RollRotMatrix * PitchRotMatrix * YawRotMatrix;
}

FVector4 FTransform::GetForwardDirection() const
{
    return FVector4(1, 0, 0, 1) * GetRotationMatrix();
}

UPBRMatParams::~UPBRMatParams()
{
    delete BaseColor;
    delete Metallic;
    delete Roughness;
    delete Emissive;
    delete AO;
}

FVector4 UPBRMatParams::SampleBaseColor(const FVector2& UV) const
{
    if (nullptr == BaseColor) return BaseColorDefault;
    return BaseColor->SampleWithBiLinear(UV);
}

FVector4 UPBRMatParams::SampleMetallic(const FVector2& UV) const
{
    if (nullptr == Metallic) return MetallicDefault;
    return Metallic->SampleWithBiLinear(UV);
}

FVector4 UPBRMatParams::SampleRoughness(const FVector2& UV) const
{
    if (nullptr == Roughness) return RoughnessDefault;
    return Roughness->SampleWithBiLinear(UV);
}

FVector4 UPBRMatParams::SampleEmissive(const FVector2& UV) const
{
    if (nullptr == Emissive) return EmissiveDefault;
    return Emissive->SampleWithBiLinear(UV);
}

FVector4 UPBRMatParams::SampleAO(const FVector2& UV) const
{
    if (nullptr == AO) return AODefault;
    return AO->SampleWithBiLinear(UV);
}

FVector4 UPBRMatParams::SampleCombinedTexture(const FVector2& UV) const
{
    if(nullptr == RMSCombineTexture) return RMSCombineDefault;
    return RMSCombineTexture->SampleWithBiLinear(UV);
}

FVector4 UMaterialParams::SampleDiffuse(const FVector2& UV) const
{
    if (!DiffuseTexture) return {1.f, 1.f, 1.f, 0.1f};
    return DiffuseTexture->SampleWithBiLinear(UV);
}

FVector4 UMaterialParams::SampleSpecular(const FVector2& UV) const
{
    if (!SpecularTexture) return {0.1f, 0.1f, 0.1f, 0.1f};
    return SpecularTexture->SampleWithBiLinear(UV);
}

FVector4 UMaterialParams::SampleNormal(const FVector2& UV, const FVector4& Default) const
{
    if (!NormalTexture) return Default;
    return NormalTexture->SampleWithBiLinear(UV);
}

UMaterialParams::UMaterialParams()
{
    PBRParams = new UPBRMatParams;
}

UMaterialParams::~UMaterialParams()
{
    delete PBRParams;
    delete DiffuseTexture;
    delete SpecularTexture;
    delete NormalTexture;
}

#pragma once
#include <cstdlib>
#include <vector>
#include <string>


struct FMatrix4x4;
struct FVector4;
class UShaderBase;
class UTexture;
class ULightBase;
class UShadowMappingPass;

struct FVector2
{
    float X;
    float Y;

    FVector2()
    {
        X = 0.f;
        Y = 0.f;
    }

    FVector2(float InX, float InY)
    {
        X = InX;
        Y = InY;
    }

    FVector2 operator+(const FVector2& Other) const
    {
        FVector2 Ret;
        Ret.X = X + Other.X;
        Ret.Y = Y + Other.Y;
        return Ret;
    }

    FVector2 operator-(const FVector2& Other) const
    {
        FVector2 Ret;
        Ret.X = X - Other.X;
        Ret.Y = Y - Other.Y;
        return Ret;
    }

    FVector2 operator*(const FVector2& Other) const
    {
        FVector2 Ret;
        Ret.X = X * Other.X;
        Ret.Y = Y * Other.Y;
        return Ret;
    }

    FVector2 operator*(float Value) const
    {
        FVector2 Ret;
        Ret.X = X * Value;
        Ret.Y = Y * Value;
        return Ret;
    }

    FVector2 operator/(const FVector2& Other) const
    {
        FVector2 Ret;
        Ret.X = X / Other.X;
        Ret.Y = Y / Other.Y;
        return Ret;
    }

    float DotProduct(const FVector2& Other) const
    {
        return X * Other.X + Y * Other.Y;
    }

    float CrossProduct(const FVector2& Other) const
    {
        return Y * Other.X - Other.Y * X;
    }

    FVector2 Lerp(const FVector2& B, float Alpha) const
    {
        return *this + (B - *this) * Alpha;
    }
};

struct FVector3
{
    float X;
    float Y;
    float Z;

    FVector3()
    {
        X = 0.f;
        Y = 0.f;
        Z = 0.f;
    }

    FVector3(float InX, float InY, float InZ)
    {
        X = InX;
        Y = InY;
        Z = InZ;
    }

    FVector3(const FVector4& Vect4);


    FVector3 operator+(const FVector3& Other) const
    {
        FVector3 Ret;
        Ret.X = X + Other.X;
        Ret.Y = Y + Other.Y;
        Ret.Z = Z + Other.Z;
        return Ret;
    }

    FVector3 operator-(const FVector3& Other) const
    {
        FVector3 Ret;
        Ret.X = X - Other.X;
        Ret.Y = Y - Other.Y;
        Ret.Z = Z - Other.Z;
        return Ret;
    }

    FVector3 operator*(float Value) const
    {
        FVector3 Ret;
        Ret.X = X * Value;
        Ret.Y = Y * Value;
        Ret.Z = Z * Value;
        return Ret;
    }

    FVector3 operator*(const FVector3& Other) const
    {
        FVector3 Ret;
        Ret.X = X * Other.X;
        Ret.Y = Y * Other.Y;
        Ret.Z = Z * Other.Z;
        return Ret;
    }

    FVector3 operator/(const FVector3& Other) const
    {
        FVector3 Ret;
        Ret.X = X / Other.X;
        Ret.Y = Y / Other.Y;
        Ret.Z = Z / Other.Z;
        return Ret;
    }

    float DotProduct(const FVector3& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    FVector3 CrossProduct(const FVector3& Other) const
    {
        FVector3 Ret;
        Ret.X = Y * Other.Z - Z * Other.Y;
        Ret.Y = Z * Other.X - X * Other.Z;
        Ret.Z = X * Other.Y - Y * Other.X;
        return Ret;
    }

    FVector3 Lerp(const FVector3& B, float Alpha) const
    {
        return *this + (B - *this) * Alpha;
    }

    float Len() const
    {
        return sqrt(X * X + Y * Y + Z * Z);
    }

    FVector3& Normalize()
    {
        const float L = Len();
        X /= L;
        Y /= L;
        Z /= L;
        return *this;
    }

    FVector3& operator/=(float Value)
    {
        X /= Value;
        Y /= Value;
        Z /= Value;
        return *this;
    }


    FVector3 operator/(float Value) const
    {
        FVector3 Ret;
        Ret.X = X / Value;
        Ret.Y = Y / Value;
        Ret.Z = Z / Value;
        return Ret;
    }

    FVector3& operator*=(float Value)
    {
        X *= Value;
        Y *= Value;
        Z *= Value;
        return *this;
    }
    
    FVector3& operator*=(const FVector4& Value);
    
    bool IsNan() const
    {
        if(isnan(Y)) return true;
        if(isnan(X)) return true;
        if(isnan(Z)) return true;
        return false;
    }
};

struct FVector4
{
    float X;
    float Y;
    float Z;
    float W;

    FVector4()
    {
        X = 0.f;
        Y = 0.f;
        Z = 0.f;
        W = 0.f;
    }

    FVector4(const FVector3& Vector3)
    {
        X = Vector3.X;
        Y = Vector3.Y;
        Z = Vector3.Z;
        W = 1.0f;
    }

    FVector4(float InX, float InY, float InZ, float InW)
    {
        X = InX;
        Y = InY;
        Z = InZ;
        W = InW;
    }

    FVector4 TransformVector(const FMatrix4x4& InMatrix) const;

    FVector4 operator+(const FVector4& Other) const
    {
        FVector4 Ret;
        Ret.X = X + Other.X;
        Ret.Y = Y + Other.Y;
        Ret.Z = Z + Other.Z;
        Ret.W = W + Other.W;
        return Ret;
    }

    FVector4 operator-(const FVector4& Other) const
    {
        FVector4 Ret;
        Ret.X = X - Other.X;
        Ret.Y = Y - Other.Y;
        Ret.Z = Z - Other.Z;
        Ret.W = W - Other.W;
        return Ret;
    }

    FVector4 operator*(float Value) const
    {
        FVector4 Ret;
        Ret.X = X * Value;
        Ret.Y = Y * Value;
        Ret.Z = Z * Value;
        Ret.W = W * Value;
        return Ret;
    }

    FVector4 operator*(const FVector4& Other) const
    {
        FVector4 Ret;
        Ret.X = X * Other.X;
        Ret.Y = Y * Other.Y;
        Ret.Z = Z * Other.Z;
        Ret.W = W * Other.W;
        return Ret;
    }

    FVector4 operator*(const FMatrix4x4& InMatrix) const;

    FVector4 operator/(const FVector4& Other) const
    {
        FVector4 Ret;
        Ret.X = X / Other.X;
        Ret.Y = Y / Other.Y;
        Ret.Z = Z / Other.Z;
        Ret.W = W / Other.W;
        return Ret;
    }


    float DotProduct(const FVector4& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
    }

    FVector3 CrossProduct(const FVector4& Other) const
    {
        FVector3 Ret;
        Ret.X = Y * Other.Z - Z * Other.Y;
        Ret.Y = Z * Other.X - X * Other.Z;
        Ret.Z = X * Other.Y - Y * Other.X;
        return Ret;
    }

    FVector4 operator/(float Value) const
    {
        FVector4 Ret;
        Ret.X = X / Value;
        Ret.Y = Y / Value;
        Ret.Z = Z / Value;
        Ret.W = W / Value;
        return Ret;
    }

    FVector4 Lerp(const FVector4& B, float Alpha) const
    {
        return *this + (B - *this) * Alpha;
    }

    FVector4& operator+=(const FVector4& Tmp)
    {
        X += Tmp.X;
        Y += Tmp.Y;
        Z += Tmp.Z;
        W += Tmp.W;
        return *this;
    }

    float Len() const
    {
        return sqrt(X * X + Y * Y + Z * Z + W * W);
    }

    void Normalize()
    {
        const float L = Len();
        X = X / L;
        Y = Y / L;
        Z = Z / L;
        W = W / L;
    }

    FVector4& operator/=(float Value);

    FVector4& operator/=(const FVector4& Other)
    {
        X /= Other.X;
        Y /= Other.Y;
        Z /= Other.Z;
        W /= Other.W;
        return *this;
    }

    void Clamp(float Min, float Max)
    {
        if (X > Max) X = Max;
        if (Y > Max) Y = Max;
        if (Z > Max) Z = Max;
        if (W > Max) W = Max;

        if (X < Min) X = Min;
        if (Y < Min) Y = Min;
        if (Z < Min) Z = Min;
        if (W < Min) W = Min;
    }
};

struct FSize
{
    int X;
    int Y;

    FSize()
    {
        X = 0;
        Y = 0;
    }

    FSize(int InX, int InY)
    {
        X = InX;
        Y = InY;
    }

    FSize operator-(int i) const
    {
        return FSize(X - i, Y - i);
    }

    FSize operator+(int i) const
    {
        return FSize(X + i, Y + i);
    }

    FSize operator-(const FSize& Other) const
    {
        return FSize(X - Other.X, Y - Other.Y);
    }

    FSize operator+(const FSize& Other) const
    {
        return FSize(X + Other.X, Y + Other.Y);
    }

    FSize& operator*=(int Value)
    {
        X = X * Value;
        Y = Y * Value;
        return *this;
    }

    FSize operator*(int Value) const
    {
        FSize Ret;
        Ret.X = X * Value;
        Ret.Y = Y * Value;
        return Ret;
    }
};

#define Matrix4x4N 4

struct FMatrix4x4
{
    FVector4 XPlane;
    FVector4 YPlane;
    FVector4 ZPlane;
    FVector4 WPlane;

    FMatrix4x4()
    {
        XPlane.X = 1;
        YPlane.Y = 1;
        ZPlane.Z = 1;
        WPlane.W = 1;
    }

    FMatrix4x4 operator*(const FMatrix4x4& Other)
    {
        const FMatrix4x4 OtherT = Other.Transpose();

        FMatrix4x4 Ret;

        Ret.XPlane.X = XPlane.DotProduct(OtherT.XPlane);
        Ret.XPlane.Y = XPlane.DotProduct(OtherT.YPlane);
        Ret.XPlane.Z = XPlane.DotProduct(OtherT.ZPlane);
        Ret.XPlane.W = XPlane.DotProduct(OtherT.WPlane);

        Ret.YPlane.X = YPlane.DotProduct(OtherT.XPlane);
        Ret.YPlane.Y = YPlane.DotProduct(OtherT.YPlane);
        Ret.YPlane.Z = YPlane.DotProduct(OtherT.ZPlane);
        Ret.YPlane.W = YPlane.DotProduct(OtherT.WPlane);

        Ret.ZPlane.X = ZPlane.DotProduct(OtherT.XPlane);
        Ret.ZPlane.Y = ZPlane.DotProduct(OtherT.YPlane);
        Ret.ZPlane.Z = ZPlane.DotProduct(OtherT.ZPlane);
        Ret.ZPlane.W = ZPlane.DotProduct(OtherT.WPlane);

        Ret.WPlane.X = WPlane.DotProduct(OtherT.XPlane);
        Ret.WPlane.Y = WPlane.DotProduct(OtherT.YPlane);
        Ret.WPlane.Z = WPlane.DotProduct(OtherT.ZPlane);
        Ret.WPlane.W = WPlane.DotProduct(OtherT.WPlane);

        return Ret;
    }

    FVector3 TransformVector(const FVector4& InVector) const
    {
        FVector3 Ret;
        Ret.X = InVector.X * XPlane.X + InVector.Y * YPlane.X + InVector.Z * ZPlane.X;
        Ret.Y = InVector.X * XPlane.Y + InVector.Y * YPlane.Y + InVector.Z * ZPlane.Y;
        Ret.Z = InVector.X * XPlane.Z + InVector.Y * YPlane.Z + InVector.Z * ZPlane.Z;
        return Ret;
    }


    FMatrix4x4 Transpose() const
    {
        FMatrix4x4 Ret;

        Ret.XPlane.X = XPlane.X;
        Ret.XPlane.Y = YPlane.X;
        Ret.XPlane.Z = ZPlane.X;
        Ret.XPlane.W = WPlane.X;

        Ret.YPlane.X = XPlane.Y;
        Ret.YPlane.Y = YPlane.Y;
        Ret.YPlane.Z = ZPlane.Y;
        Ret.YPlane.W = WPlane.Y;

        Ret.ZPlane.X = XPlane.Z;
        Ret.ZPlane.Y = YPlane.Z;
        Ret.ZPlane.Z = ZPlane.Z;
        Ret.ZPlane.W = WPlane.Z;

        Ret.WPlane.X = XPlane.W;
        Ret.WPlane.Y = YPlane.W;
        Ret.WPlane.Z = ZPlane.W;
        Ret.WPlane.W = WPlane.W;

        return Ret;
    }

    FMatrix4x4 GetInverse() const
    {
        FMatrix4x4 Ret;

        float ArrayMatrix[Matrix4x4N][Matrix4x4N];
        float ArrayInverseMatrix[Matrix4x4N][Matrix4x4N];
        GetToArrayMatrix(ArrayMatrix);
        if (Inverse(ArrayMatrix, ArrayInverseMatrix))
        {
            Ret.XPlane.X = ArrayInverseMatrix[0][0];
            Ret.XPlane.Y = ArrayInverseMatrix[0][1];
            Ret.XPlane.Z = ArrayInverseMatrix[0][2];
            Ret.XPlane.W = ArrayInverseMatrix[0][3];


            Ret.YPlane.X = ArrayInverseMatrix[1][0];
            Ret.YPlane.Y = ArrayInverseMatrix[1][1];
            Ret.YPlane.Z = ArrayInverseMatrix[1][2];
            Ret.YPlane.W = ArrayInverseMatrix[1][3];


            Ret.ZPlane.X = ArrayInverseMatrix[2][0];
            Ret.ZPlane.Y = ArrayInverseMatrix[2][1];
            Ret.ZPlane.Z = ArrayInverseMatrix[2][2];
            Ret.ZPlane.W = ArrayInverseMatrix[2][3];


            Ret.WPlane.X = ArrayInverseMatrix[3][0];
            Ret.WPlane.Y = ArrayInverseMatrix[3][1];
            Ret.WPlane.Z = ArrayInverseMatrix[3][2];
            Ret.WPlane.W = ArrayInverseMatrix[3][3];
        }
        return Ret;
    }

    void GetToArrayMatrix(float (& OutArray)[4][4]) const
    {
        OutArray[0][0] = XPlane.X;
        OutArray[0][1] = XPlane.Y;
        OutArray[0][2] = XPlane.Z;
        OutArray[0][3] = XPlane.W;

        OutArray[1][0] = YPlane.X;
        OutArray[1][1] = YPlane.Y;
        OutArray[1][2] = YPlane.Z;
        OutArray[1][3] = YPlane.W;


        OutArray[2][0] = ZPlane.X;
        OutArray[2][1] = ZPlane.Y;
        OutArray[2][2] = ZPlane.Z;
        OutArray[2][3] = ZPlane.W;


        OutArray[3][0] = WPlane.X;
        OutArray[3][1] = WPlane.Y;
        OutArray[3][2] = WPlane.Z;
        OutArray[3][3] = WPlane.W;
    }


    static bool Inverse(float (*a)[Matrix4x4N], float (*b)[Matrix4x4N])
    {
        using namespace std;
        int i, j, k;
        float max, temp;
        // 定义一个临时矩阵t
        float t[Matrix4x4N][Matrix4x4N];
        // 将a矩阵临时存放在矩阵t[n][n]中
        for (i = 0; i < Matrix4x4N; i++)
        {
            for (j = 0; j < Matrix4x4N; j++)
            {
                t[i][j] = a[i][j];
            }
        }
        // 初始化B矩阵为单位矩阵
        for (i = 0; i < Matrix4x4N; i++)
        {
            for (j = 0; j < Matrix4x4N; j++)
            {
                b[i][j] = (i == j) ? (float)1 : 0;
            }
        }
        // 进行列主消元，找到每一列的主元
        for (i = 0; i < Matrix4x4N; i++)
        {
            max = t[i][i];
            // 用于记录每一列中的第几个元素为主元
            k = i;
            // 寻找每一列中的主元元素
            for (j = i + 1; j < Matrix4x4N; j++)
            {
                if (fabs(t[j][i]) > fabs(max))
                {
                    max = t[j][i];
                    k = j;
                }
            }
            //cout<<"the max number is "<<max<<endl;
            // 如果主元所在的行不是第i行，则进行行交换
            if (k != i)
            {
                // 进行行交换
                for (j = 0; j < Matrix4x4N; j++)
                {
                    temp = t[i][j];
                    t[i][j] = t[k][j];
                    t[k][j] = temp;
                    // 伴随矩阵B也要进行行交换
                    temp = b[i][j];
                    b[i][j] = b[k][j];
                    b[k][j] = temp;
                }
            }
            if (t[i][i] == 0)
            {
                return false;
            }
            // 获取列主元素
            temp = t[i][i];
            // 将主元所在的行进行单位化处理
            //cout<<"\nthe temp is "<<temp<<endl;
            for (j = 0; j < Matrix4x4N; j++)
            {
                t[i][j] = t[i][j] / temp;
                b[i][j] = b[i][j] / temp;
            }
            for (j = 0; j < Matrix4x4N; j++)
            {
                if (j != i)
                {
                    temp = t[j][i];
                    //消去该列的其他元素
                    for (k = 0; k < Matrix4x4N; k++)
                    {
                        t[j][k] = t[j][k] - temp * t[i][k];
                        b[j][k] = b[j][k] - temp * b[i][k];
                    }
                }
            }
        }
        return true;
    }
};


struct FTransform
{
private:
    FVector3 Loc;
    FVector3 Rot;
    FVector3 Scale3D = FVector3(1.f, 1.f, 1.f);

    mutable FMatrix4x4 Matrix;

    mutable bool bDirty = true;

public:
    void SetLocation(const FVector3& InLoc);

    void SetRotation(const FVector3& InRotation);

    void SetScale3D(const FVector3& InScale);

    const FVector3& GetRotation() const;

    FVector4 GetForwardDirection() const;

    const FMatrix4x4& GetMatrix() const;

    const FVector3& GetLocation() const { return Loc; }

protected:
    FMatrix4x4 GetRotationMatrix() const;
};

struct FTriangleFace
{
    int Vertices[3] = {0};
    int UVs[3] = {0};
    int Normals[3] = {0};

    FVector3 Normal;
    FVector3 Tangent;
    FVector3 BiTangent;
};

class UPBRMatParams
{
public:
    ~UPBRMatParams();

    UTexture* BaseColor = nullptr;
    UTexture* Metallic = nullptr;
    UTexture* Roughness = nullptr;
    UTexture* Emissive = nullptr;
    UTexture* AO = nullptr;

    /*
    * Roughness Metalic Specular 组成一张贴图,分别是 x, y
    */
    UTexture* RMSCombineTexture = nullptr;

    float Shiness = 1.0f;

    FVector4 SampleBaseColor(const FVector2& UV) const;
    FVector4 SampleMetallic(const FVector2& UV) const;
    FVector4 SampleRoughness(const FVector2& UV) const;
    FVector4 SampleEmissive(const FVector2& UV) const;
    FVector4 SampleAO(const FVector2& UV) const;

    FVector4 SampleCombinedTexture(const FVector2& UV) const;

    bool IsCombinedTexture()
    {
        return nullptr != RMSCombineTexture;
    }

protected:
    
    FVector4 BaseColorDefault = {1, 1, 1, 1};
    FVector4 MetallicDefault = {0.4f, 0.4f, 0.4f, 1};
    FVector4 RoughnessDefault = {0.8f, 0.8f, 0.8f, 1};
    FVector4 EmissiveDefault = {0, 0, 0, 1};
    FVector4 AODefault = {1, 1, 1, 1};
    FVector4 RMSCombineDefault = {0.8f, 0.4f, 1.f, 1.f};
};

class UMaterialParams
{
public:
    UTexture* DiffuseTexture = nullptr;
    UTexture* SpecularTexture = nullptr;
    UTexture* NormalTexture = nullptr;

    UPBRMatParams* PBRParams = nullptr;

    FVector4 AmbientAbsorbance;

    UMaterialParams();

    ~UMaterialParams();

    FVector4 SampleDiffuse(const FVector2& UV) const;
    FVector4 SampleSpecular(const FVector2& UV) const;
    FVector4 SampleNormal(const FVector2& UV, const FVector4& Default) const;
};


struct FModelData
{
    std::vector<int> Indices;

    std::vector<FVector2> UVs;

    std::vector<FVector3> Vertices;

    std::vector<FVector3> Normals;

    std::vector<FTriangleFace> Faces;
};

struct FRenderModelProxy
{
    ~FRenderModelProxy()
    {
        free(ModelData);
        free(Shader);
        free(MatParams);
    }

    FModelData* ModelData = nullptr;
    FTransform Transform;
    UShaderBase* Shader = nullptr;
    UMaterialParams* MatParams = nullptr;
};

struct FVSOutput
{
    FVector4 Pos;
    FVector4 WorldPos;
    FVector3 WorldNormal;
    FVector2 UV;
    float Depth = 0;

    FVSOutput Lerp(const FVSOutput& B, float Alpha) const
    {
        FVSOutput Ret;
        Ret.Pos = this->Pos.Lerp(B.Pos, Alpha);
        Ret.WorldPos = this->WorldPos.Lerp(B.WorldPos, Alpha);
        Ret.WorldNormal = this->WorldNormal.Lerp(B.WorldNormal, Alpha);
        Ret.UV = this->UV.Lerp(B.UV, Alpha);
        Ret.Depth = Depth + (B.Depth - Depth) * Alpha;
        return Ret;
    }
};

class UShaderGlobal
{
public:
    std::vector<ULightBase*> Lights;

    FVector4 AmbientColor;

    FTransform CameraTrans;

    UShadowMappingPass* ShadowMapping;
};

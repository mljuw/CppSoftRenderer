#include "WaveFrontLoader.h"

#include <fstream>

FModelData* UWaveFrontLoader::LoadFromFile(std::string FilePath)
{
    
    std::ifstream FileIn(FilePath.c_str());

    if (!FileIn.good()) return nullptr;
    FModelData* Ret = new FModelData;
    constexpr int BufferSize = 1024 * 1024; 
    char* Buffer = static_cast<char*>(malloc(BufferSize));
    memset(Buffer, 0, BufferSize);
    
    while (!FileIn.getline(Buffer, BufferSize).eof())
    {
        // std::string Line(Buffer, sizeof(Buffer));
        char& FirstChar = Buffer[0];
        if (FirstChar == 'v')
        {
            if (Buffer[1] == 'n')
            {
                float X, Y, Z;
                if(3 == sscanf(Buffer, "vn %f %f %f", &X, &Y, &Z))
                    Ret->Normals.push_back(FVector3(X, Y, Z));
            }
            else if (Buffer[1] == 't')
            {
                float U, V;
                if(2 == sscanf(Buffer, "vt %f %f", &U, &V)) 
                    Ret->UVs.push_back({U, 1.f - V});
            }
            else
            {
                float X, Y, Z;
                if(3 == sscanf(Buffer, "v %f %f %f", &X, &Y, &Z));
                    Ret->Vertices.push_back({X, Y, Z});
            }
        }
        else if (FirstChar == 'f')
        {
            FTriangleFace Face;
            sscanf(Buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &Face.Vertices[0], &Face.UVs[0], &Face.Normals[0],
                &Face.Vertices[1], &Face.UVs[1], &Face.Normals[1],
                &Face.Vertices[2], &Face.UVs[2], &Face.Normals[2]
                );
            for(int& Idx : Face.Vertices)
            {
                Idx = Idx - 1;
            }
            for(int& Idx : Face.Normals)
            {
                Idx = Idx - 1;
            }
            
            for(int& Idx : Face.UVs)
            {
                Idx = Idx - 1;
            }
            
            Ret->Faces.push_back(Face);
        }
    }

    for(FTriangleFace& Face : Ret->Faces)
    { 
        const int VIndex1 = Face.Vertices[0];
        const int VIndex2 = Face.Vertices[1];
        const int VIndex3 = Face.Vertices[2];
        const int UVIndex1 = Face.UVs[0];
        const int UVIndex2 = Face.UVs[1];
        const int UVIndex3 = Face.UVs[2];

        const FVector3& V1 = Ret->Vertices[VIndex1];
        const FVector3& V2 = Ret->Vertices[VIndex2];
        const FVector3& V3 = Ret->Vertices[VIndex3];

        const FVector2& UV1 = Ret->UVs[UVIndex1];
        const FVector2& UV2 = Ret->UVs[UVIndex2];
        const FVector2& UV3 = Ret->UVs[UVIndex3];

        Face.Normal = (V2 - V1).Normalize().CrossProduct(V3 - V1);
        Face.Normal.Normalize();
        
        if(isnan(Face.Normal.X))
        {
            Face.Normal = Ret->Vertices[Face.Normals[0]];
        }
        
        CalcTangentAndBiTangent(V1, V2, V3, UV1, UV2, UV3, Face.Normal, Face.Tangent, Face.BiTangent);
    }
    
    FileIn.close();
    free(Buffer);
    
    return Ret;
}


void UWaveFrontLoader::CalcTangentAndBiTangent(const FVector3& V1, const FVector3& V2, const FVector3& V3, const FVector2& UV1, const FVector2& UV2, const FVector2& UV3, const FVector3& Normal, FVector3& OutTangent, FVector3& OutBiTangent)
{
    const FVector3 Edge1 = V2 - V1;
    const FVector3 Edge2 = V3 - V2;
    const FVector2 DeltaUV1 = UV2 - UV1;
    const FVector2 DeltaUV2 = UV3 - UV2;


    const float DivNum = DeltaUV1.X * DeltaUV2.Y - DeltaUV2.X * DeltaUV1.Y;

    if(DivNum == 0.f)
    {
        return;
    }

    const float f = 1.0f / DivNum;

    OutTangent.X = f * (DeltaUV2.Y * Edge1.X - DeltaUV1.Y * Edge2.X);
    OutTangent.Y = f * (DeltaUV2.Y * Edge1.Y - DeltaUV1.Y * Edge2.Y);
    OutTangent.Z = f * (DeltaUV2.Y * Edge1.Z - DeltaUV1.Y * Edge2.Z);
    OutTangent.Normalize();

    OutBiTangent = Normal.CrossProduct(OutTangent);
    OutBiTangent.Normalize();

    //BiTangent.X = f * (-DeltaUV2.X * Edge1.X - DeltaUV1.X * Edge2.X);
    //BiTangent.Y = f * (-DeltaUV2.X * Edge1.Y - DeltaUV1.X * Edge2.Y);
    //BiTangent.Z = f * (-DeltaUV2.X * Edge1.Z - DeltaUV1.X * Edge2.Z);
    //BiTangent.Normalize();

}
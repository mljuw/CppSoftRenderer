#include "BlinnPhongShader.h"
#include "BaseStruct.h"
#include "LightBase.h"
#include "ShadowMappingPass.h"
#include "UTexture.h"

FVector4 UBlinnPhongShader::FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams, const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos, const FVector2& UV, const float Depth) const
{
	//I = Ia * Ka + Ip * Kd * (L Dot N) + Ip * Ks * (H * N)^n

	const FVector4 Diffuse = MatParams->SampleDiffuse(UV);
	const FVector4 Specular = MatParams->SampleSpecular(UV);
	FVector4 TexNormal = MatParams->SampleNormal(UV, Normal);

	TexNormal = BTNMatrix.TransformVector(TexNormal);
	
	FVector4 Ret = ShaderGlobal->AmbientColor * MatParams->AmbientAbsorbance * Diffuse;

	FVector4 V = (ShaderGlobal->CameraTrans.GetLocation() - WorldPos);
	V.Normalize();
	
	FVector4 Id;
	FVector4 Is;
	for (int i = 0; i < ShaderGlobal->Lights.size(); ++i)
	{
		const ULightBase* Light = ShaderGlobal->Lights[i];
		const FVector3 LightDir = Light->GetLightDirection(WorldPos);

		FVector3 H = V + LightDir;
		H.Normalize();
		const float HDotN = std::max<float>(0.f, H.DotProduct(TexNormal));
		const float LDotN = std::max<float>(0.f, LightDir.DotProduct(TexNormal));

		const float AttenuationFactor = pow(Light->GetAttenuationFactor(WorldPos), 2);
		Id += (Light->LightColor * Diffuse * LDotN * AttenuationFactor);

		Is += (Light->LightColor * Specular * pow(HDotN, 2));
	}
	const float ShadowAttenuation =  ShaderGlobal->ShadowMapping->GetShadowAttenuation(WorldPos);
	return (Ret + Id + Is) * ShadowAttenuation;
	
	//return MatParams->DiffuseTexture->SampleWithBiLinear(UV);
	//return {Depth, Depth, Depth, Depth};
}
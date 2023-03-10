#include "ShaderBase.h"
#include "BaseStruct.h"

FVSOutput UShaderBase::VertexShader(const FVector3& Vertex, const FVector3& Normal, const FVector2& UV,
	const FMatrix4x4& ModelMatrix, const FMatrix4x4& VPMatrix,
	const FMatrix4x4& NormalToWorldMatrix) const
{
	FVSOutput VertexOutput;
	VertexOutput.WorldPos = FVector4(Vertex) * ModelMatrix;
	VertexOutput.Pos = VertexOutput.WorldPos * VPMatrix;
	//VertexOutput.Depth = VertexOutput.Pos.W;
	VertexOutput.WorldNormal = NormalToWorldMatrix.TransformVector(Normal);
	VertexOutput.WorldNormal.Normalize();
	VertexOutput.UV = UV;

	return VertexOutput;
}

FVector4 UShaderBase::FragmentShader(UShaderGlobal* ShaderGlobal, UMaterialParams* MatParams, const FMatrix4x4& BTNMatrix, const FVector4& Normal, const FVector4& WorldPos, const FVector2& UV, const float Depth) const
{
	return FVector4(1, 1, 1, 1);
}
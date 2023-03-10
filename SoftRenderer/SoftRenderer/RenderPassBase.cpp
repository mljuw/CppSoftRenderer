#include "RenderPassBase.h"

#include "Renderer.h"
#include "ShaderBase.h"

void URenderPassBase::SetPerspectiveProjection(float Fov, float Near, float Far, float Ratio)
{
	const float HalfFov = Fov / 2;
	const float HalfHeight = tan(HalfFov * 3.141592654f / 180.f) * Near;
	const float HalfWidth = HalfHeight * Ratio;

	const float F_N = Far - Near;
	ProjectionMatrix.XPlane = { Far / F_N, 0, 0, 1 };
	ProjectionMatrix.YPlane = { 0, 1 / HalfWidth, 0, 0 };
	ProjectionMatrix.ZPlane = { 0, 0, 1 / HalfHeight, 0 };
	ProjectionMatrix.WPlane = { -Far * Near / F_N, 0, 0, 0 };
	ProjectionType = EProjectionType::Perspective;

	MaxDepth = F_N;
}

void URenderPassBase::SetOrthoProjection(float Near, float Far, float Width, float Ratio)
{
	const float Height = Width / Ratio;

	const float F_N = Far - Near;
	ProjectionMatrix.XPlane = { 1 / F_N, 0, 0, 0 };
	ProjectionMatrix.YPlane = { 0, 2 / Width, 0, 0 };
	ProjectionMatrix.ZPlane = { 0, 0, 2 / Height, 0 };
	ProjectionMatrix.WPlane = { -Near / F_N, 0, 0, 1 };

	ProjectionType = EProjectionType::Ortho;

	MaxDepth = F_N;
}

bool URenderPassBase::IsBackFace(const FVector4& V1, const FVector4& V2, const FVector4& V3) const
{
	FVector4 Normal = (V1 - V2).CrossProduct(V3 - V2);
	Normal.Normalize();
	return Normal.DotProduct({ -1, 0, 0, 0 }) > 0.f;

		/*const float Ret = (V1.Z * V2.Y) - (V1.Y * V2.Z) + (V1.Y * V3.Z) - (V1.Z * V3.Y) - (V2.Y * V3.Z) + (V2.Z * V3.Y);
		return Ret < 0;*/
}

void URenderPassBase::OnScreenSizeChange()
{
	CalcNDC2ScreenMatrix();
}

void URenderPassBase::CalcNDC2ScreenMatrix()
{
	const FSize& ScreenSize = Renderer->GetScreenSize();
	NDC2ScreenMatrix.YPlane.Y = ScreenSize.X / 2.f;
	NDC2ScreenMatrix.ZPlane.Z = (-ScreenSize.Y) / 2.f;
	NDC2ScreenMatrix.WPlane.Y = ScreenSize.X / 2.f;
	NDC2ScreenMatrix.WPlane.Z = ScreenSize.Y / 2.f;
}

void URenderPassBase::CalcBarycentricCoordinateParams(const FSize& V1, const FSize& V2, const FSize& V3,
	const FVector2& G, FVector3& OutParams) const
{
	const float Y2_Y3 = V2.Y - V3.Y;
	const float X_X3 = G.X - V3.X;
	const float Y_Y3 = G.Y - V3.Y;
	const float X2_X3 = V2.X - V3.X;
	const float X1_X3 = V1.X - V3.X;
	const float Y1_Y3 = V1.Y - V3.Y;
	const float TotalArea = Y2_Y3 * X1_X3 - Y1_Y3 * X2_X3;
	if(0 >= TotalArea)
	{
		OutParams.X = -1;
		OutParams.Y = -1;
		OutParams.Z = -1;
		return;
	}
	OutParams.X = (Y2_Y3 * X_X3 - Y_Y3 * X2_X3) / TotalArea;
	OutParams.Y = (Y_Y3 * X1_X3 - Y1_Y3 * X_X3) / TotalArea;
	OutParams.Z = 1.f - OutParams.X - OutParams.Y;
}

bool URenderPassBase::CheckPixelInTriangle(const FVector3& AlphaBetaGamma) const
{
	if (AlphaBetaGamma.X < 0 || AlphaBetaGamma.Y < 0 || AlphaBetaGamma.Z < 0)
		return false;

	if (AlphaBetaGamma.X > 1 || AlphaBetaGamma.Y > 1 || AlphaBetaGamma.Z > 1)
		return false;

	return true;
}

void URenderPassBase::CalcTriangleAABB(const FSize* (&Points)[3], FSize& OutMin, FSize& OutMax) const
{
	for (int i = 0; i < 3; ++i)
	{
		const FSize* Cur = Points[i];
		if (0 == i)
		{
			OutMax.X = Cur->X;
			OutMax.Y = Cur->Y;

			OutMin.X = Cur->X;
			OutMin.Y = Cur->Y;
		}
		else
		{
			if (OutMin.X > Cur->X)
				OutMin.X = Cur->X;

			if (OutMin.Y > Cur->Y)
				OutMin.Y = Cur->Y;

			if (OutMax.X < Cur->X)
				OutMax.X = Cur->X;

			if (OutMax.Y < Cur->Y)
				OutMax.Y = Cur->Y;
		}
	}
}

void URenderPassBase::PrePipeline()
{
	switch (Renderer->GetAAType())
	{
	case EAntiAliasingType::None:
		CurrentAAPixelOffset = &NoneAAPixelOffset;
		break;
	default:
		CurrentAAPixelOffset = &AAPixelOffset;
		break;
	}

	CameraMatrix = Renderer->GetCameraTransform().GetMatrix().GetInverse();
}

void URenderPassBase::Rasterization(FVSOutput (&Vertices)[3],
	FRenderModelProxy* Model, const FVector3& WorldTangent, const FVector3& WorldBiTangent)
{
	const FVSOutput& V1 = Vertices[0];
	const FVSOutput& V2 = Vertices[1];
	const FVSOutput& V3 = Vertices[2];
	
	const FSize V1PixelPos = FSize(V1.Pos.Y, V1.Pos.Z);
	const FSize V2PixelPos = FSize(V2.Pos.Y, V2.Pos.Z);
	const FSize V3PixelPos = FSize(V3.Pos.Y, V3.Pos.Z);

	const FSize* Points[3] = { &V1PixelPos, &V2PixelPos, &V3PixelPos };
	FSize Max, Min;
	CalcTriangleAABB(Points, Min, Max);

	FVector4 Color;
	float Depth;
	for (int PixelY = Min.Y; PixelY < Max.Y; ++PixelY)
	{
		for (int PixelX = Min.X; PixelX < Max.X; ++PixelX)
		{
			for (int i = 0; i < CurrentAAPixelOffset->size(); ++i)
			{
				const FVector2& Offset = (*CurrentAAPixelOffset)[i];
				const float X = PixelX + Offset.X;
				const float Y = PixelY + Offset.Y;
				FVector3 AlphaBetaGamma;
				CalcBarycentricCoordinateParams(V1PixelPos, V2PixelPos, V3PixelPos, { X, Y }, AlphaBetaGamma);
				
				if (CheckPixelInTriangle(AlphaBetaGamma))
				{
					if (Renderer->GetAAType() == EAntiAliasingType::MSAA)
					{
						EnterMSAAInfo.push_back({ i, AlphaBetaGamma, X, Y });
						continue;
					}
					UFrameBuffer* CurFrameBuffer = Renderer->GetFrameBuffer(i);

					if (CheckAndUpdateDepth(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, AlphaBetaGamma, PixelX, PixelY, Depth))
					{
						FragmentProcess(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, AlphaBetaGamma, PixelX, PixelY, WorldTangent, WorldBiTangent, Depth, Color);

						CurFrameBuffer->ColorBuffer->SetData(X, Y, Color);
					}
				}
			}

			if (Renderer->GetAAType() == EAntiAliasingType::MSAA && EnterMSAAInfo.size() > 0)
			{
				bool bCalcFragment = false;
				for (int i = 0 ; i < EnterMSAAInfo.size(); ++i)
				{
					MSAACulcInfo& Info = EnterMSAAInfo[i];
					UFrameBuffer* CurFrameBuffer = Renderer->GetFrameBuffer(Info.FrameBufferIndex);
					if (!bCalcFragment)
					{

						if(CheckAndUpdateDepth(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, Info.AlphaBetaGamma, Info.X, Info.Y, Depth))
						{
							FragmentProcess(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, Info.AlphaBetaGamma, PixelX, PixelY, WorldTangent, WorldBiTangent, Depth, Color);

							CurFrameBuffer->ColorBuffer->SetData(PixelX, PixelY, Color);
							bCalcFragment = true;
						}
					}
					else
					{
						if(CheckAndUpdateDepth(Model->Shader, CurFrameBuffer, Model->MatParams, V1, V2, V3, Info.AlphaBetaGamma, Info.X, Info.Y, Depth))
							CurFrameBuffer->ColorBuffer->SetData(PixelX, PixelY, Color);
					}

				}
				EnterMSAAInfo.clear();
			}

		}
	}
}

void URenderPassBase::CalcFluoroscopicCorrectionParams(const FVector3& AlphaBetaGamma, float V1Depth, float V2Depth,
	float V3Depth, float& OutXt, float(&OutParams)[3]) const
{
	OutParams[0] = AlphaBetaGamma.X / V1Depth;
	OutParams[1] = AlphaBetaGamma.Y / V2Depth;
	OutParams[2] = AlphaBetaGamma.Z / V3Depth;
	OutXt = 1.f / (OutParams[0] + OutParams[1] + OutParams[2]);
}

void URenderPassBase::FragmentProcess(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
	const FVector3& AlphaBetaGamma, int X, int Y, const FVector4& WorldTangent, const FVector4& WorldBiTangent, float Depth, FVector4& OutColor)
{
}



bool URenderPassBase::CheckAndUpdateDepth(UShaderBase* Shader, UFrameBuffer* FrameBuffer, UMaterialParams* MatParams, const FVSOutput& V1, const FVSOutput& V2, const FVSOutput& V3,
	const FVector3& AlphaBetaGamma, int X, int Y, float& OutDepth)
{
	return true;
}


bool URenderPassBase::CheckPointInClipPlane(EClipPlaneType PlaneType, const FVector4& CVVPoint) const
{
	switch (PlaneType)
	{
	case EClipPlaneType::W:
		return CVVPoint.W > SMALL_NUMBER;
	case EClipPlaneType::Bottom:
		return CVVPoint.Z >= -CVVPoint.W;
	case EClipPlaneType::Top:
		return CVVPoint.Z <= CVVPoint.W;
	case EClipPlaneType::Far:
		return CVVPoint.X <= CVVPoint.W;
	case EClipPlaneType::Near:
		return CVVPoint.X >= 0;
	case EClipPlaneType::Left:
		return CVVPoint.Y >= -CVVPoint.W;
	case EClipPlaneType::Right:
		return CVVPoint.Y <= CVVPoint.W;
	}
	return false;
}

void URenderPassBase::ClipTriangleWithPlaneType(EClipPlaneType PlaneType, std::vector<FVSOutput>& Vertices) const
{
	std::vector<FVSOutput> TempArray;
	int PreIndex = Vertices.size() - 1;
	for (int i = 0; i < Vertices.size(); ++i)
	{
		if (0 != i) PreIndex = i - 1;

		const FVSOutput& PrePoint = Vertices[PreIndex];
		const FVSOutput& CurPoint = Vertices[i];

		const bool bPreInPlane = CheckPointInClipPlane(PlaneType, PrePoint.Pos);
		const bool bCurInPlane = CheckPointInClipPlane(PlaneType, CurPoint.Pos);

		if (bPreInPlane != bCurInPlane)
		{
			const FVector4& Pre = PrePoint.Pos;
			const FVector4& Cur = CurPoint.Pos;
			float Alpha = 0.f;
			if (PlaneType == EClipPlaneType::Top)
			{
				Alpha = (Pre.W - Pre.Z) / (Pre.W - Cur.W - (Pre.Z - Cur.Z));
			}
			else if (PlaneType == EClipPlaneType::Bottom)
			{
				Alpha = (Pre.Z + Pre.W) / (Pre.W - Cur.W + (Pre.Z - Cur.Z));
			}
			else if (PlaneType == EClipPlaneType::Near)
			{
				Alpha = Pre.X / (Pre.X - Cur.X);
			}
			else if (PlaneType == EClipPlaneType::Far)
			{
				Alpha = (Pre.W - Pre.X) / (Pre.W - Cur.W - (Pre.X - Cur.X));
			}
			else if (PlaneType == EClipPlaneType::Left)
			{
				Alpha = (Pre.W + Pre.Y) / (Pre.W - Cur.W + (Pre.Y - Cur.Y));
			}
			else if (PlaneType == EClipPlaneType::Right)
			{
				Alpha = (Pre.W - Pre.Y) / (Pre.W - Cur.W - (Pre.Y - Cur.Y));
			}
			else if (PlaneType == EClipPlaneType::W)
			{
				Alpha = (Pre.W - SMALL_NUMBER) / (Pre.W - Cur.W);
			}
			TempArray.push_back(PrePoint.Lerp(CurPoint, Alpha));
		}

		if (bCurInPlane)
		{
			TempArray.push_back(CurPoint);
		}
	}
	Vertices = TempArray;
}

void URenderPassBase::BeginPipeline(FRenderModelProxy* Model)
{
	const FMatrix4x4 ModelMatrix = Model->Transform.GetMatrix();
	const FMatrix4x4 ModelTranspose = ModelMatrix.Transpose();
	const FMatrix4x4 NormalToWorldMatrix = ModelTranspose.GetInverse();
	const FMatrix4x4& VPMatrix = GetViewProjectionMatrix();
	FVSOutput NewVertices[3];
	for (const FTriangleFace& Face : Model->ModelData->Faces)
	{
		std::vector<FVSOutput> Vertices;
		for (int i = 0; i < 3; ++i)
		{
			const int VertexIndex = Face.Vertices[i];
			const int NormalIndex = Face.Normals[i];
			const int UVIndex = Face.UVs[i];
			FVector3& Vertex = Model->ModelData->Vertices[VertexIndex];
			FVector3& Normal = Model->ModelData->Normals[NormalIndex];
			FVector2& UV = Model->ModelData->UVs[UVIndex];
			FVSOutput VertexOutput = Model->Shader->VertexShader(Vertex, Normal, UV, Model->Transform.GetMatrix(),
				VPMatrix, NormalToWorldMatrix);
			Vertices.push_back(VertexOutput);
		}

		FVector3 WorldTangent = Model->Transform.GetMatrix().TransformVector(Face.Tangent);
		WorldTangent.Normalize();
		FVector3 WorldBiTangent = Model->Transform.GetMatrix().TransformVector(Face.BiTangent);
		WorldBiTangent.Normalize();

		ClipTriangleWithPlaneType(EClipPlaneType::W, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Bottom, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Top, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Near, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Far, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Left, Vertices);
		ClipTriangleWithPlaneType(EClipPlaneType::Right, Vertices);

		if (Vertices.size() <= 0) continue;

		const int I1 = 0;
		int I2, I3;

		for (int i = 0; i < Vertices.size() - 2; ++i)
		{
			I2 = i + 1;
			I3 = i + 2;

			NewVertices[0] = Vertices[I1];
			NewVertices[1] = Vertices[I2];
			NewVertices[2] = Vertices[I3];

			FVSOutput& V1 = NewVertices[0];
			FVSOutput& V2 = NewVertices[1];
			FVSOutput& V3 = NewVertices[2];

			if(ProjectionType == EProjectionType::Perspective)
			{
				for (FVSOutput& Vertex : NewVertices)
				{
					Vertex.Depth = Vertex.Pos.X / MaxDepth;
					Vertex.Pos = Vertex.Pos / Vertex.Pos.W;
				}
			}
			else
			{
				for (FVSOutput& Vertex : NewVertices)
				{
					Vertex.Depth = Vertex.Pos.X;
				}
			}

			if (IsBackFace(V1.Pos, V2.Pos, V3.Pos))
				continue;

			Rasterization(NewVertices, Model, WorldTangent, WorldBiTangent);
		}
	}
}

void URenderPassBase::PostPipeline()
{
}

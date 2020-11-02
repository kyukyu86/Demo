// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#include "WRKismetMeshSliceLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "SkeletalMeshRenderData.h"
#include "GeomTools.h"
#include "Engine/DataTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WREnpcTable.h"
#include "Define/WRTypeDefine.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/EnvActor/SlicedCharacter/WRSlicedActor.h"
#include "Manager/WRConsoleManager.h"
#include "Core/WRGameInstance.h"

/** Util that returns 1 ir on positive side of plane, -1 if negative, or 0 if split by plane */
int32 CustomBoxPlaneCompare(FBox InBox, const FPlane& InPlane)
{
	FVector BoxCenter, BoxExtents;
	InBox.GetCenterAndExtents(BoxCenter, BoxExtents);

	// Find distance of box center from plane
	float BoxCenterDist = InPlane.PlaneDot(BoxCenter);

	// See size of box in plane normal direction
	float BoxSize = FVector::BoxPushOut(InPlane, BoxExtents);

	if (BoxCenterDist > BoxSize)
	{
		return 1;
	}
	else if (BoxCenterDist < -BoxSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/** Take two static mesh verts and interpolate all values between them */
FProcMeshVertex CustomInterpolateVert(const FProcMeshVertex& V0, const FProcMeshVertex& V1, float Alpha)
{
	FProcMeshVertex Result;

	// Handle dodgy alpha
	if (FMath::IsNaN(Alpha) || !FMath::IsFinite(Alpha))
	{
		Result = V1;
		return Result;
	}

	Result.Position = FMath::Lerp(V0.Position, V1.Position, Alpha);

	Result.Normal = FMath::Lerp(V0.Normal, V1.Normal, Alpha);

	Result.Tangent.TangentX = FMath::Lerp(V0.Tangent.TangentX, V1.Tangent.TangentX, Alpha);
	Result.Tangent.bFlipTangentY = V0.Tangent.bFlipTangentY; // Assume flipping doesn't change along edge...

	Result.UV0 = FMath::Lerp(V0.UV0, V1.UV0, Alpha);

	Result.Color.R = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.R), float(V1.Color.R), Alpha)), 0, 255);
	Result.Color.G = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.G), float(V1.Color.G), Alpha)), 0, 255);
	Result.Color.B = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.B), float(V1.Color.B), Alpha)), 0, 255);
	Result.Color.A = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.A), float(V1.Color.A), Alpha)), 0, 255);

	return Result;
}

/** Transform triangle from 2D to 3D static-mesh triangle. */
void CustomTransform2DPolygonTo3D(const FUtilPoly2D& InPoly, const FMatrix& InMatrix, TArray<FProcMeshVertex>& OutVerts, FBox& OutBox)
{
	FVector PolyNormal = -InMatrix.GetUnitAxis(EAxis::Z);
	FProcMeshTangent PolyTangent(InMatrix.GetUnitAxis(EAxis::X), false);

	for (int32 VertexIndex = 0; VertexIndex < InPoly.Verts.Num(); VertexIndex++)
	{
		const FUtilVertex2D& InVertex = InPoly.Verts[VertexIndex];

		FProcMeshVertex NewVert;

		NewVert.Position = InMatrix.TransformPosition(FVector(InVertex.Pos.X, InVertex.Pos.Y, 0.f));
		NewVert.Normal = PolyNormal;
		NewVert.Tangent = PolyTangent;
		NewVert.Color = InVertex.Color;
		NewVert.UV0 = InVertex.UV;

		OutVerts.Add(NewVert);

		// Update bounding box
		OutBox += NewVert.Position;
	}
}

/** Given a polygon, decompose into triangles. */
bool CustomTriangulatePoly(TArray<uint32>& OutTris, const TArray<FProcMeshVertex>& PolyVerts, int32 VertBase, const FVector& PolyNormal)
{
	// Can't work if not enough verts for 1 triangle
	int32 NumVerts = PolyVerts.Num() - VertBase;
	if (NumVerts < 3)
	{
		OutTris.Add(0);
		OutTris.Add(2);
		OutTris.Add(1);

		// Return true because poly is already a tri
		return true;
	}

	// Remember initial size of OutTris, in case we need to give up and return to this size
	const int32 TriBase = OutTris.Num();

	// Init array of vert indices, in order. We'll modify this
	TArray<int32> VertIndices;
	VertIndices.AddUninitialized(NumVerts);
	for (int VertIndex = 0; VertIndex < NumVerts; VertIndex++)
	{
		VertIndices[VertIndex] = VertBase + VertIndex;
	}

	// Keep iterating while there are still vertices
	while (VertIndices.Num() >= 3)
	{
		// Look for an 'ear' triangle
		bool bFoundEar = false;
		for (int32 EarVertexIndex = 0; EarVertexIndex < VertIndices.Num(); EarVertexIndex++)
		{
			// Triangle is 'this' vert plus the one before and after it
			const int32 AIndex = (EarVertexIndex == 0) ? VertIndices.Num() - 1 : EarVertexIndex - 1;
			const int32 BIndex = EarVertexIndex;
			const int32 CIndex = (EarVertexIndex + 1) % VertIndices.Num();

			const FProcMeshVertex& AVert = PolyVerts[VertIndices[AIndex]];
			const FProcMeshVertex& BVert = PolyVerts[VertIndices[BIndex]];
			const FProcMeshVertex& CVert = PolyVerts[VertIndices[CIndex]];

			// Check that this vertex is convex (cross product must be positive)
			const FVector ABEdge = BVert.Position - AVert.Position;
			const FVector ACEdge = CVert.Position - AVert.Position;
			const float TriangleDeterminant = (ABEdge ^ ACEdge) | PolyNormal;
			if (TriangleDeterminant > 0.f)
			{
				continue;
			}

			bool bFoundVertInside = false;
			// Look through all verts before this in array to see if any are inside triangle
			for (int32 VertexIndex = 0; VertexIndex < VertIndices.Num(); VertexIndex++)
			{
				const FProcMeshVertex& TestVert = PolyVerts[VertIndices[VertexIndex]];

				if (VertexIndex != AIndex &&
					VertexIndex != BIndex &&
					VertexIndex != CIndex &&
					FGeomTools::PointInTriangle(AVert.Position, BVert.Position, CVert.Position, TestVert.Position))
				{
					bFoundVertInside = true;
					break;
				}
			}

			// Triangle with no verts inside - its an 'ear'! 
			if (!bFoundVertInside)
			{
				OutTris.Add(VertIndices[AIndex]);
				OutTris.Add(VertIndices[CIndex]);
				OutTris.Add(VertIndices[BIndex]);

				// And remove vertex from polygon
				VertIndices.RemoveAt(EarVertexIndex);

				bFoundEar = true;
				break;
			}
		}

		// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
		if (!bFoundEar)
		{
			OutTris.SetNum(TriBase, true);
			return false;
		}
	}

	return true;
}

/** Util to slice a convex hull with a plane */
void CustomSliceConvexElem(const FKConvexElem& InConvex, const FPlane& SlicePlane, TArray<FVector>& OutConvexVerts)
{
	// Get set of planes that make up hull
	TArray<FPlane> ConvexPlanes;
	InConvex.GetPlanes(ConvexPlanes);

	if (ConvexPlanes.Num() >= 4)
	{
		// Add on the slicing plane (need to flip as it culls geom in the opposite sense to our geom culling code)
		ConvexPlanes.Add(SlicePlane.Flip());

		// Create output convex based on new set of planes
		FKConvexElem SlicedElem;
		bool bSuccess = SlicedElem.HullFromPlanes(ConvexPlanes, InConvex.VertexData);
		if (bSuccess)
		{
			OutConvexVerts = SlicedElem.VertexData;
		}
	}
}

int32 WRKismetMeshSliceLibrary::GetNewIndexForOldVertIndexFromSkinnedMesh(int32 MeshVertIndex, TMap<int32, int32>& MeshToSectionVertMap, USkeletalMeshComponent* InSkelMeshComp, const class FSkeletalMeshLODRenderData& InLOD, class FSkinWeightVertexBuffer& WeightBuffer, TArray<FVector>& Vertices, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<struct FProcMeshTangent>& Tangents)
{
	int32* NewIndexPtr = MeshToSectionVertMap.Find(MeshVertIndex);
	const FStaticMeshVertexBuffers& VertexBuffers = InLOD.StaticVertexBuffers;
	if (NewIndexPtr != nullptr)
	{
		return *NewIndexPtr;
	}
	else
	{
		// Copy position
		int32 SectionVertIndex = Vertices.Add(InSkelMeshComp->GetSkinnedVertexPosition(InSkelMeshComp, MeshVertIndex, InLOD, WeightBuffer));

		// Copy normal
		Normals.Add(VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(MeshVertIndex));
		check(Normals.Num() == Vertices.Num());

		// Copy UVs
		UVs.Add(VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(MeshVertIndex, 0));
		check(UVs.Num() == Vertices.Num());

		// Copy tangents
		FVector4 TangentX = VertexBuffers.StaticMeshVertexBuffer.VertexTangentX(MeshVertIndex);
		FProcMeshTangent NewTangent(TangentX, TangentX.W < 0.f);

		Tangents.Add(NewTangent);
		check(Tangents.Num() == Vertices.Num());

		MeshToSectionVertMap.Add(MeshVertIndex, SectionVertIndex);

		return SectionVertIndex;
	}
}

void WRKismetMeshSliceLibrary::GetIndexBufferFromSkeletalMesh(FMultiSizeIndexContainerData& InIndexBufferData, USkeletalMesh * InSkeletalMesh, int32 LODIndex)
{
	InIndexBufferData.DataTypeSize = InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex].MultiSizeIndexContainer.GetDataTypeSize();
	InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex].MultiSizeIndexContainer.GetIndexBuffer(InIndexBufferData.Indices);
}

void WRKismetMeshSliceLibrary::GetSectionFromSkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, const TArray<uint32>& InIndices, int32 InSectionIndex)
{
	USkeletalMesh * InSkeletalMesh = SkeletalMeshComponent->SkeletalMesh;

	if (InSkeletalMesh == nullptr)
		return;

	if (InSkeletalMesh->GetResourceForRendering() != nullptr &&
		InSkeletalMesh->GetResourceForRendering()->LODRenderData.IsValidIndex(LODIndex))
	{
		const FSkeletalMeshLODRenderData& InLOD = InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex];

		FSkinWeightVertexBuffer* WeightBuffer = SkeletalMeshComponent->GetSkinWeightBuffer(LODIndex);

		// Empty output buffers
		Vertices.Reset();
		Triangles.Reset();
		Normals.Reset();
		UVs.Reset();
		Tangents.Reset();

		// Map from vert buffer for whole mesh to vert buffer for section of interest
		TMap<int32, int32> MeshToSectionVertMap;

		FMultiSizeIndexContainerData CurrentMultiSizeIndexContainerData;

		CurrentMultiSizeIndexContainerData.DataTypeSize = InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex].MultiSizeIndexContainer.GetDataTypeSize();
		InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex].MultiSizeIndexContainer.GetIndexBuffer(CurrentMultiSizeIndexContainerData.Indices);


		const FSkelMeshRenderSection& InSections = SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[LODIndex].RenderSections[InSectionIndex];

		const uint32 OnePastLastIndex = InSections.BaseIndex + InSections.NumTriangles * 3;

		for (uint32 inIndex = InSections.BaseIndex; inIndex < OnePastLastIndex; inIndex++)
		{
			uint32 MeshVertIndex = InIndices[inIndex];

			int32 SectionVertIndex = GetNewIndexForOldVertIndexFromSkinnedMesh(MeshVertIndex, MeshToSectionVertMap, SkeletalMeshComponent, InLOD, *WeightBuffer, Vertices, Normals, UVs, Tangents);

			// Add to index buffer
			Triangles.Add(SectionVertIndex);
		}

		//uint32 MaxIndiceCount = InIndices.Num();

		//for (uint32 i = 0; i < MaxIndiceCount; i++)
		//{
		//	uint32 MeshVertIndex = InIndices[i];

		//	int32 SectionVertIndex = GetNewIndexForOldVertIndexFromSkinnedMesh(MeshVertIndex, MeshToSectionVertMap, SkeletalMeshComponent, InLOD, *WeightBuffer, Vertices, Normals, UVs, Tangents);

		//	// Add to index buffer
		//	Triangles.Add(SectionVertIndex);
		//}
	}
}

bool WRKismetMeshSliceLibrary::CopyProceduralMeshFromSkeletalMeshComponent(USkeletalMeshComponent * SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent * ProcMeshComponent, bool bCreateCollision)
{
	if (SkeletalMeshComponent != nullptr &&
		SkeletalMeshComponent->SkeletalMesh != nullptr)
	{

		USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->SkeletalMesh;

		FMultiSizeIndexContainerData CurrentMultiSizeIndexContainerData;
		GetIndexBufferFromSkeletalMesh(CurrentMultiSizeIndexContainerData, SkeletalMesh, LODIndex);

		uint32 NumSections = SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[LODIndex].RenderSections.Num();

		for (uint32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
		{
			TArray<FVector> Vertices;
			TArray<int32> Triangles;
			TArray<FVector> Normals;
			TArray<FVector2D> UVs;
			TArray<FVector2D> UVs1;
			TArray<FVector2D> UVs2;
			TArray<FVector2D> UVs3;
			TArray<FProcMeshTangent> Tangents;

			GetSectionFromSkeletalMesh(SkeletalMeshComponent, LODIndex, Vertices, Triangles, Normals, UVs, Tangents, CurrentMultiSizeIndexContainerData.Indices, SectionIndex);

			TArray<FLinearColor> DummyColors;

			ProcMeshComponent->CreateMeshSection_LinearColor(SectionIndex, Vertices, Triangles, Normals, UVs, UVs1, UVs2, UVs3, DummyColors, Tangents, false);
			//}

		}
		ProcMeshComponent->ClearCollisionConvexMeshes();

		if (SkeletalMeshComponent->BodySetup != nullptr)
		{
			// Iterate over all convex hulls on skeletal mesh..
			const int32 NumConvex = SkeletalMeshComponent->BodySetup->AggGeom.ConvexElems.Num();
			for (int ConvexIndex = 0; ConvexIndex < NumConvex; ConvexIndex++)
			{
				// Copy convex verts to ProcMesh
				FKConvexElem& MeshConvex = SkeletalMeshComponent->BodySetup->AggGeom.ConvexElems[ConvexIndex];
				ProcMeshComponent->AddCollisionConvexMesh(MeshConvex.VertexData);
			}
		}

		for (int32 MatIndex = 0; MatIndex < SkeletalMeshComponent->GetNumMaterials(); MatIndex++)
		{
			ProcMeshComponent->SetMaterial(MatIndex, SkeletalMeshComponent->GetMaterial(MatIndex));
		}

		ProcMeshComponent->SetWorldTransform(SkeletalMeshComponent->GetComponentTransform());

		ProcMeshComponent->RegisterComponent();

		return true;
	}
	return false;
}

void WRKismetMeshSliceLibrary::CustomSliceProceduralMesh(UProceduralMeshComponent * InProcMesh, FVector PlanePosition, FVector PlaneNormal, bool bCreateOtherHalf, UProceduralMeshComponent *& OutOtherHalfProcMesh, EProcMeshSliceCapOption CapOption, UMaterialInterface * CapMaterial, AActor * HalfActor/* = nullptr*/)
{
	if (InProcMesh != nullptr)
	{
		// Transform plane from world to local space
		FTransform ProcCompToWorld = InProcMesh->GetComponentToWorld();
		FVector LocalPlanePos = ProcCompToWorld.InverseTransformPosition(PlanePosition);
		FVector LocalPlaneNormal = ProcCompToWorld.InverseTransformVectorNoScale(PlaneNormal);
		LocalPlaneNormal = LocalPlaneNormal.GetSafeNormal(); // Ensure normalized

		FPlane SlicePlane(LocalPlanePos, LocalPlaneNormal);

		// Set of sections to add to the 'other half' component
		TArray<FProcMeshSection> OtherSections;
		// Material for each section of other half
		TArray<UMaterialInterface*> OtherMaterials;

		// Set of new edges created by clipping polys by plane
		TArray<FUtilEdge3D> ClipEdges;

		for (int32 SectionIndex = 0; SectionIndex < InProcMesh->GetNumSections(); SectionIndex++)
		{
			FProcMeshSection* BaseSection = InProcMesh->GetProcMeshSection(SectionIndex);
			// If we have a section, and it has some valid geom
			if (BaseSection != nullptr && BaseSection->ProcIndexBuffer.Num() > 0 && BaseSection->ProcVertexBuffer.Num() > 0)
			{
				// Compare bounding box of section with slicing plane
				int32 BoxCompare = CustomBoxPlaneCompare(BaseSection->SectionLocalBox, SlicePlane);

				// Box totally clipped, clear section
				if (BoxCompare == -1)
				{
					// Add entire section to other half
					if (bCreateOtherHalf)
					{
						OtherSections.Add(*BaseSection);
						OtherMaterials.Add(InProcMesh->GetMaterial(SectionIndex));
					}

					InProcMesh->ClearMeshSection(SectionIndex);
				}
				// Box totally on one side of plane, leave it alone, do nothing
				else if (BoxCompare == 1)
				{
					// ...
				}
				// Box intersects plane, need to clip some polys!
				else
				{
					// New section for geometry
					FProcMeshSection NewSection;

					// New section for 'other half' geometry (if desired)
					FProcMeshSection* NewOtherSection = nullptr;
					if (bCreateOtherHalf)
					{
						int32 OtherSectionIndex = OtherSections.Add(FProcMeshSection());
						NewOtherSection = &OtherSections[OtherSectionIndex];

						OtherMaterials.Add(InProcMesh->GetMaterial(SectionIndex)); // Remember material for this section
					}

					// Map of base vert index to sliced vert index
					TMap<int32, int32> BaseToSlicedVertIndex;
					TMap<int32, int32> BaseToOtherSlicedVertIndex;

					const int32 NumBaseVerts = BaseSection->ProcVertexBuffer.Num();

					// Distance of each base vert from slice plane
					TArray<float> VertDistance;
					VertDistance.AddUninitialized(NumBaseVerts);

					// Build vertex buffer 
					for (int32 BaseVertIndex = 0; BaseVertIndex < NumBaseVerts; BaseVertIndex++)
					{
						FProcMeshVertex& BaseVert = BaseSection->ProcVertexBuffer[BaseVertIndex];

						// Calc distance from plane
						VertDistance[BaseVertIndex] = SlicePlane.PlaneDot(BaseVert.Position);

						// See if vert is being kept in this section
						if (VertDistance[BaseVertIndex] > 0.f)
						{
							// Copy to sliced v buffer
							int32 SlicedVertIndex = NewSection.ProcVertexBuffer.Add(BaseVert);
							// Update section bounds
							NewSection.SectionLocalBox += BaseVert.Position;
							// Add to map
							BaseToSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
						}
						// Or add to other half if desired
						else if (NewOtherSection != nullptr)
						{
							int32 SlicedVertIndex = NewOtherSection->ProcVertexBuffer.Add(BaseVert);
							NewOtherSection->SectionLocalBox += BaseVert.Position;
							BaseToOtherSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
						}
					}


					// Iterate over base triangles (ie 3 indices at a time)
					for (int32 BaseIndex = 0; BaseIndex < BaseSection->ProcIndexBuffer.Num(); BaseIndex += 3)
					{
						int32 BaseV[3]; // Triangle vert indices in original mesh
						int32* SlicedV[3]; // Pointers to vert indices in new v buffer
						int32* SlicedOtherV[3]; // Pointers to vert indices in new 'other half' v buffer

						// For each vertex..
						for (int32 i = 0; i < 3; i++)
						{
							// Get triangle vert index
							BaseV[i] = BaseSection->ProcIndexBuffer[BaseIndex + i];
							// Look up in sliced v buffer
							SlicedV[i] = BaseToSlicedVertIndex.Find(BaseV[i]);
							// Look up in 'other half' v buffer (if desired)
							if (bCreateOtherHalf)
							{
								SlicedOtherV[i] = BaseToOtherSlicedVertIndex.Find(BaseV[i]);
								// Each base vert _must_ exist in either BaseToSlicedVertIndex or BaseToOtherSlicedVertIndex 
								check((SlicedV[i] != nullptr) != (SlicedOtherV[i] != nullptr));
							}
						}

						// If all verts survived plane cull, keep the triangle
						if (SlicedV[0] != nullptr && SlicedV[1] != nullptr && SlicedV[2] != nullptr)
						{
							NewSection.ProcIndexBuffer.Add(*SlicedV[0]);
							NewSection.ProcIndexBuffer.Add(*SlicedV[1]);
							NewSection.ProcIndexBuffer.Add(*SlicedV[2]);
						}
						// If all verts were removed by plane cull
						else if (SlicedV[0] == nullptr && SlicedV[1] == nullptr && SlicedV[2] == nullptr)
						{
							// If creating other half, add all verts to that
							if (NewOtherSection != nullptr)
							{
								NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[0]);
								NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[1]);
								NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[2]);
							}
						}
						// If partially culled, clip to create 1 or 2 new triangles
						else
						{
							int32 FinalVerts[4];
							int32 NumFinalVerts = 0;

							int32 OtherFinalVerts[4];
							int32 NumOtherFinalVerts = 0;

							FUtilEdge3D NewClipEdge;
							int32 ClippedEdges = 0;

							float PlaneDist[3];
							PlaneDist[0] = VertDistance[BaseV[0]];
							PlaneDist[1] = VertDistance[BaseV[1]];
							PlaneDist[2] = VertDistance[BaseV[2]];

							for (int32 EdgeIdx = 0; EdgeIdx < 3; EdgeIdx++)
							{
								int32 ThisVert = EdgeIdx;

								// If start vert is inside, add it.
								if (SlicedV[ThisVert] != nullptr)
								{
									check(NumFinalVerts < 4);
									FinalVerts[NumFinalVerts++] = *SlicedV[ThisVert];
								}
								// If not, add to other side
								else if (bCreateOtherHalf)
								{
									check(NumOtherFinalVerts < 4);
									OtherFinalVerts[NumOtherFinalVerts++] = *SlicedOtherV[ThisVert];
								}

								// If start and next vert are on opposite sides, add intersection
								int32 NextVert = (EdgeIdx + 1) % 3;

								if ((SlicedV[EdgeIdx] == nullptr) != (SlicedV[NextVert] == nullptr))
								{
									// Find distance along edge that plane is
									float Alpha = -PlaneDist[ThisVert] / (PlaneDist[NextVert] - PlaneDist[ThisVert]);
									// Interpolate vertex params to that point
									FProcMeshVertex InterpVert = CustomInterpolateVert(BaseSection->ProcVertexBuffer[BaseV[ThisVert]], BaseSection->ProcVertexBuffer[BaseV[NextVert]], FMath::Clamp(Alpha, 0.0f, 1.0f));

									// Add to vertex buffer
									int32 InterpVertIndex = NewSection.ProcVertexBuffer.Add(InterpVert);
									// Update bounds
									NewSection.SectionLocalBox += InterpVert.Position;

									// Save vert index for this poly
									check(NumFinalVerts < 4);
									FinalVerts[NumFinalVerts++] = InterpVertIndex;

									// If desired, add to the poly for the other half as well
									if (NewOtherSection != nullptr)
									{
										int32 OtherInterpVertIndex = NewOtherSection->ProcVertexBuffer.Add(InterpVert);
										NewOtherSection->SectionLocalBox += InterpVert.Position;
										check(NumOtherFinalVerts < 4);
										OtherFinalVerts[NumOtherFinalVerts++] = OtherInterpVertIndex;
									}

									// When we make a new edge on the surface of the clip plane, save it off.
									check(ClippedEdges < 2);
									if (ClippedEdges == 0)
									{
										NewClipEdge.V0 = InterpVert.Position;
									}
									else
									{
										NewClipEdge.V1 = InterpVert.Position;
									}

									ClippedEdges++;
								}
							}

							// Triangulate the clipped polygon.
							for (int32 VertexIndex = 2; VertexIndex < NumFinalVerts; VertexIndex++)
							{
								NewSection.ProcIndexBuffer.Add(FinalVerts[0]);
								NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex - 1]);
								NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex]);
							}

							// If we are making the other half, triangulate that as well
							if (NewOtherSection != nullptr)
							{
								for (int32 VertexIndex = 2; VertexIndex < NumOtherFinalVerts; VertexIndex++)
								{
									NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[0]);
									NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[VertexIndex - 1]);
									NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[VertexIndex]);
								}
							}

							check(ClippedEdges != 1); // Should never clip just one edge of the triangle

							// If we created a new edge, save that off here as well
							if (ClippedEdges == 2)
							{
								ClipEdges.Add(NewClipEdge);
							}
						}
					}

					// Remove 'other' section from array if no valid geometry for it
					if (NewOtherSection != nullptr && (NewOtherSection->ProcIndexBuffer.Num() == 0 || NewOtherSection->ProcVertexBuffer.Num() == 0))
					{
						OtherSections.RemoveAt(OtherSections.Num() - 1);
					}

					// If we have some valid geometry, update section
					if (NewSection.ProcIndexBuffer.Num() > 0 && NewSection.ProcVertexBuffer.Num() > 0)
					{
						// Assign new geom to this section
						InProcMesh->SetProcMeshSection(SectionIndex, NewSection);
					}
					// If we don't, remove this section
					else
					{
						InProcMesh->ClearMeshSection(SectionIndex);
					}
				}
			}
		}

		// Create cap geometry (if some edges to create it from)
		if (CapOption != EProcMeshSliceCapOption::NoCap && ClipEdges.Num() > 0)
		{
			FProcMeshSection CapSection;
			int32 CapSectionIndex = INDEX_NONE;

			// If using an existing section, copy that info first
			if (CapOption == EProcMeshSliceCapOption::UseLastSectionForCap)
			{
				CapSectionIndex = InProcMesh->GetNumSections() - 1;
				CapSection = *InProcMesh->GetProcMeshSection(CapSectionIndex);
			}
			// Adding new section for cap
			else
			{
				CapSectionIndex = InProcMesh->GetNumSections();
			}

			// Project 3D edges onto slice plane to form 2D edges
			TArray<FUtilEdge2D> Edges2D;
			FUtilPoly2DSet PolySet;
			FGeomTools::ProjectEdges(Edges2D, PolySet.PolyToWorld, ClipEdges, SlicePlane);

			// Find 2D closed polygons from this edge soup
			FGeomTools::Buid2DPolysFromEdges(PolySet.Polys, Edges2D, FColor(255, 255, 255, 255));

			// Remember start point for vert and index buffer before adding and cap geom
			int32 CapVertBase = CapSection.ProcVertexBuffer.Num();
			int32 CapIndexBase = CapSection.ProcIndexBuffer.Num();

			// Triangulate each poly
			for (int32 PolyIdx = 0; PolyIdx < PolySet.Polys.Num(); PolyIdx++)
			{
				// Generate UVs for the 2D polygon.
				FGeomTools::GeneratePlanarTilingPolyUVs(PolySet.Polys[PolyIdx], 64.f);

				// Remember start of vert buffer before adding triangles for this poly
				int32 PolyVertBase = CapSection.ProcVertexBuffer.Num();

				// Transform from 2D poly verts to 3D
				CustomTransform2DPolygonTo3D(PolySet.Polys[PolyIdx], PolySet.PolyToWorld, CapSection.ProcVertexBuffer, CapSection.SectionLocalBox);

				// Triangulate this polygon
				CustomTriangulatePoly(CapSection.ProcIndexBuffer, CapSection.ProcVertexBuffer, PolyVertBase, LocalPlaneNormal);
			}

			// Set geom for cap section
			InProcMesh->SetProcMeshSection(CapSectionIndex, CapSection);

			// If creating new section for cap, assign cap material to it
			if (CapOption == EProcMeshSliceCapOption::CreateNewSectionForCap)
			{
				InProcMesh->SetMaterial(CapSectionIndex, CapMaterial);
			}

			// If creating the other half, copy cap geom into other half sections
			if (bCreateOtherHalf)
			{
				// Find section we want to use for the cap on the 'other half'
				FProcMeshSection* OtherCapSection;
				if (CapOption == EProcMeshSliceCapOption::CreateNewSectionForCap)
				{
					OtherSections.Add(FProcMeshSection());
					OtherMaterials.Add(CapMaterial);
				}
				OtherCapSection = &OtherSections.Last();

				// Remember current base index for verts in 'other cap section'
				int32 OtherCapVertBase = OtherCapSection->ProcVertexBuffer.Num();

				// Copy verts from cap section into other cap section
				for (int32 VertIdx = CapVertBase; VertIdx < CapSection.ProcVertexBuffer.Num(); VertIdx++)
				{
					FProcMeshVertex OtherCapVert = CapSection.ProcVertexBuffer[VertIdx];

					// Flip normal and tangent TODO: FlipY?
					OtherCapVert.Normal *= -1.f;
					OtherCapVert.Tangent.TangentX *= -1.f;

					// Add to other cap v buffer
					OtherCapSection->ProcVertexBuffer.Add(OtherCapVert);
					// And update bounding box
					OtherCapSection->SectionLocalBox += OtherCapVert.Position;
				}

				// Find offset between main cap verts and other cap verts
				int32 VertOffset = OtherCapVertBase - CapVertBase;

				// Copy indices over as well
				for (int32 IndexIdx = CapIndexBase; IndexIdx < CapSection.ProcIndexBuffer.Num(); IndexIdx += 3)
				{
					// Need to offset and change winding
					OtherCapSection->ProcIndexBuffer.Add(CapSection.ProcIndexBuffer[IndexIdx + 0] + VertOffset);
					OtherCapSection->ProcIndexBuffer.Add(CapSection.ProcIndexBuffer[IndexIdx + 2] + VertOffset);
					OtherCapSection->ProcIndexBuffer.Add(CapSection.ProcIndexBuffer[IndexIdx + 1] + VertOffset);
				}
			}
		}

		// Array of sliced collision shapes
		TArray< TArray<FVector> > SlicedCollision;
		TArray< TArray<FVector> > OtherSlicedCollision;

		UBodySetup* ProcMeshBodySetup = InProcMesh->GetBodySetup();

		for (int32 ConvexIndex = 0; ConvexIndex < ProcMeshBodySetup->AggGeom.ConvexElems.Num(); ConvexIndex++)
		{
			FKConvexElem& BaseConvex = ProcMeshBodySetup->AggGeom.ConvexElems[ConvexIndex];

			int32 BoxCompare = CustomBoxPlaneCompare(BaseConvex.ElemBox, SlicePlane);

			// If box totally clipped, add to other half (if desired)
			if (BoxCompare == -1)
			{
				if (bCreateOtherHalf)
				{
					OtherSlicedCollision.Add(BaseConvex.VertexData);
				}
			}
			// If box totally valid, just keep mesh as is
			else if (BoxCompare == 1)
			{
				SlicedCollision.Add(BaseConvex.VertexData);
			}
			// Need to actually slice the convex shape
			else
			{
				TArray<FVector> SlicedConvexVerts;
				CustomSliceConvexElem(BaseConvex, SlicePlane, SlicedConvexVerts);
				// If we got something valid, add it
				if (SlicedConvexVerts.Num() >= 4)
				{
					SlicedCollision.Add(SlicedConvexVerts);
				}

				// Slice again to get the other half of the collision, if desired
				if (bCreateOtherHalf)
				{
					TArray<FVector> OtherSlicedConvexVerts;
					CustomSliceConvexElem(BaseConvex, SlicePlane.Flip(), OtherSlicedConvexVerts);
					if (OtherSlicedConvexVerts.Num() >= 4)
					{
						OtherSlicedCollision.Add(OtherSlicedConvexVerts);
					}
				}
			}
		}

		// Update collision of proc mesh
		InProcMesh->SetCollisionConvexMeshes(SlicedCollision);

		// If creating other half, create component now
		if (bCreateOtherHalf)
		{
			// Create new component with the same outer as the proc mesh passed in

			AWRSlicedActor* InSliceHalfActor = Cast<AWRSlicedActor>(HalfActor);

			if (InSliceHalfActor == nullptr)
				return;

			OutOtherHalfProcMesh = InSliceHalfActor->GetProcMesh();

			// Set transform to match source component
			OutOtherHalfProcMesh->SetWorldTransform(InProcMesh->GetComponentTransform());

			// Add each section of geometry
			for (int32 SectionIndex = 0; SectionIndex < OtherSections.Num(); SectionIndex++)
			{
				OutOtherHalfProcMesh->SetProcMeshSection(SectionIndex, OtherSections[SectionIndex]);
				OutOtherHalfProcMesh->SetMaterial(SectionIndex, OtherMaterials[SectionIndex]);
			}

			// Copy collision settings from input mesh
			OutOtherHalfProcMesh->SetCollisionProfileName(InProcMesh->GetCollisionProfileName());
			OutOtherHalfProcMesh->SetCollisionEnabled(InProcMesh->GetCollisionEnabled());
			OutOtherHalfProcMesh->bUseComplexAsSimpleCollision = InProcMesh->bUseComplexAsSimpleCollision;

			// Assign sliced collision
			OutOtherHalfProcMesh->SetCollisionConvexMeshes(OtherSlicedCollision);

			// Finally register
			OutOtherHalfProcMesh->RegisterComponent();
		}
	}
}

void WRKismetMeshSliceLibrary::BeginSlice(AWRCharacter* OwnerChar)
{
	if (OwnerChar == nullptr)
		return;

	AWRSlicedActor* InActor = nullptr;
	AWRSlicedActor* InHalfActor = nullptr;

	UWorld* InWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (InWorld == nullptr)
		return;

	InActor = InWorld->SpawnActor<AWRSlicedActor>(AWRSlicedActor::StaticClass(), OwnerChar->GetTransform());
	InHalfActor = InWorld->SpawnActor<AWRSlicedActor>(AWRSlicedActor::StaticClass(), OwnerChar->GetTransform());

	if (InActor != nullptr && InHalfActor != nullptr)
	{
		InActor->Initialize();
		InHalfActor->Initialize();

		InActor->SetActorTransform(OwnerChar->GetTransform());
		InHalfActor->SetActorTransform(OwnerChar->GetTransform());
	}

	UProceduralMeshComponent* InProcMeshComp = InActor->GetProcMesh();
	UProceduralMeshComponent* InHalfProcMeshComp = InHalfActor->GetProcMesh();

	if (InProcMeshComp == nullptr || InHalfProcMeshComp == nullptr)
		return;

	FWREnpcTable* InEnpcTable = WRTableManager::Get()->FindRow<FWREnpcTable>(OwnerChar->GetCharacterTableID());
	if (InEnpcTable == nullptr)
		return;

	USkeletalMeshComponent* OwnerSkeletalMeshComp = OwnerChar->GetMesh();

	if (OwnerSkeletalMeshComp == nullptr)
		return;

	OwnerSkeletalMeshComp->bDisableClothSimulation = true;

	int32 CurrentLODIndex = OwnerSkeletalMeshComp->PredictedLODLevel;

	if (WRKismetMeshSliceLibrary::CopyProceduralMeshFromSkeletalMeshComponent(OwnerSkeletalMeshComp, CurrentLODIndex, InProcMeshComp, true) == false)
		return;

	InProcMeshComp->SetAllMassScale(OwnerSkeletalMeshComp->BodyInstance.MassScale);

	OwnerSkeletalMeshComp->SetVisibility(false);

	uint32 TotalSectionIndex = InProcMeshComp->GetNumSections();

	TArray<TArray<FVector>> InConvexMeshes;
	InConvexMeshes.Empty(TotalSectionIndex);
	for (uint32 InSectionIndex = 0; InSectionIndex < TotalSectionIndex; InSectionIndex++)
	{
		FProcMeshSection* InProcMeshSection = InProcMeshComp->GetProcMeshSection(InSectionIndex);

		TArray<FVector> InConvexMeshVertList;

		TArray<FProcMeshVertex> InProcMeshVerts = InProcMeshSection->ProcVertexBuffer;

		uint32 InVertsCount = InProcMeshVerts.Num();

		int32 CuttingIndex = InVertsCount % 1000;

		InVertsCount = InVertsCount - CuttingIndex;

		InConvexMeshVertList.Empty(InVertsCount / 1000);

		for (uint32 InVertIndex = 0; InVertIndex < InVertsCount; InVertIndex += 1000)
		{
			InConvexMeshVertList.Add(InProcMeshVerts[InVertIndex].Position);
		}
		InConvexMeshes.Add(InConvexMeshVertList);
	}

	InProcMeshComp->SetCollisionConvexMeshes(InConvexMeshes);

	FVector InPlaneLocation = OwnerSkeletalMeshComp->Bounds.GetBox().GetCenter(); //OwnerChar->GetActorLocation() + InLocalCenterPos;
	FVector InSliceDirection = OwnerChar->GetAttackDirection();
	InSliceDirection = InSliceDirection.RotateAngleAxis(90.f, InSliceDirection.RightVector);
	UProceduralMeshComponent* InHalfMeshComp = nullptr;

	EProcMeshSliceCapOption InCapOption;

	UMaterialInterface* InMaterial = LoadObject<UMaterialInterface>(nullptr, *InEnpcTable->SliceCapMaterial);

	if (InMaterial != nullptr)
	{
		InCapOption = EProcMeshSliceCapOption::CreateNewSectionForCap;
	}
	else
	{
		InCapOption = EProcMeshSliceCapOption::NoCap;
	}

	InProcMeshComp->SetWorldLocation(OwnerSkeletalMeshComp->GetComponentLocation());

	WRKismetMeshSliceLibrary::CustomSliceProceduralMesh(InProcMeshComp, InPlaneLocation, InSliceDirection, true, InHalfMeshComp, InCapOption, InMaterial, InHalfActor);

	if (WRConsoleManager::Get()->GetBool(EWRConsole::Slice_ActiveImpulse) == true)
	{
		float Impulse = WRConsoleManager::Get()->GetFloat(EWRConsole::Add_Impulse);
		float SliceDirectionWeight = WRConsoleManager::Get()->GetFloat(EWRConsole::Slice_DirectionWeight);

		InActor->CreateDynamicMaterials();
		InProcMeshComp->SetSimulatePhysics(true);
		InProcMeshComp->SetAllMassScale(InProcMeshComp->GetMassScale());

		InHalfActor->CreateDynamicMaterials();
		InHalfMeshComp->SetSimulatePhysics(true);
		InHalfMeshComp->SetAllMassScale(InProcMeshComp->GetMassScale());
	}
}

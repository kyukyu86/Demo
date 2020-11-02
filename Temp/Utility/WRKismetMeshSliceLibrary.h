// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KismetProceduralMeshLibrary.h"
/**
 * 
 */
class WR_API WRKismetMeshSliceLibrary : public UKismetProceduralMeshLibrary
{
public:
	WRKismetMeshSliceLibrary() {};
	~WRKismetMeshSliceLibrary() {};

private:
	static int32 GetNewIndexForOldVertIndexFromSkinnedMesh(int32 MeshVertIndex, TMap<int32, int32>& MeshToSectionVertMap, USkeletalMeshComponent* InSkelMeshComp, const class FSkeletalMeshLODRenderData& InLOD, class FSkinWeightVertexBuffer& WeightBuffer, TArray<FVector>& Vertices, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<struct FProcMeshTangent>& Tangents);
	static void GetIndexBufferFromSkeletalMesh(struct FMultiSizeIndexContainerData& InIndexBufferData, class USkeletalMesh* InSkeletalMesh, int32 LODIndex);

public:
	/** Grab geometry data from a SkeletalMesh asset. */
	UFUNCTION(BlueprintCallable, Category = "CustomProceduralMesh")
	static void GetSectionFromSkeletalMesh(class USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, const TArray<uint32>& InIndices, int32 InSectionIndex);

	/** Copy materials from SkeletalMeshComponent to ProceduralMeshComponent. */
	UFUNCTION(BlueprintCallable, Category = "CustomProceduralMesh")
	static bool CopyProceduralMeshFromSkeletalMeshComponent(class USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent, bool bCreateCollision);

	UFUNCTION(BlueprintCallable, Category = "CustomProceduralMesh")
	static void CustomSliceProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector PlanePosition, FVector PlaneNormal, bool bCreateOtherHalf, UProceduralMeshComponent*& OutOtherHalfProcMesh, EProcMeshSliceCapOption CapOption, UMaterialInterface* CapMaterial, AActor* HalfActor = nullptr);

public:
	static void BeginSlice(class AWRCharacter* OwnerChar);
};

// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
/**
 * 
 */
class WR_API WRSkeletalMeshModifier
{
public:
	WRSkeletalMeshModifier();
	~WRSkeletalMeshModifier();

private:
	// 소켓 및 constraint break
	static FName CreateNewSocketName(const class USkeleton& InSkeleton, const FName& InBoneName);
	static void CreateSkeletalSocket(USkeleton* InSkeleton, const FTransform& RelativeTransform, FName BoneName, FName NewSocketName);
	static void BreakConstraintCustom(USkeletalMeshComponent* InSkelMeshComp, FVector Impulse, FVector HitLocation, FName InBoneName, FName InJointName);
	
	// 특정 SkinWeightBuffer 에서 가장 영향이 큰 BoneIndex 받아오는 함수
	static int32 GetDominantBoneIndex(const struct FSkelMeshSkinWeightInfo& SkinWeightVert, bool bHasExtraInfluence);
	static TArray<uint8> GetBoneListBelongToBody(const USkeletalMeshComponent& InSkelMeshComp, const FName& InBoneName, int32& ControllingParentBoneIndex);

	// 충돌 바디인스턴스 받아오는 함수들
	static bool IsInsideOrOnInstance(const struct FBodyInstance* InBaseBodyInstance, const FTransform& InBaseBoneTransform, const FVector& InTargetLocation);
	static FName GetClosetBodyInstanceBone(const class USkeletalMeshComponent& InSkelMeshComp, const FVector& InHitLocation);
	static int32 GetClosetBodyInstanceIndex(const class USkeletalMeshComponent& InSkelMeshComp, const FVector& InHitLocation);
	
	// Get BoneIndexList Belong to Bodyinstance
	static FName GetJointNameForCustomConstraint(const USkeletalMeshComponent& InSkelMeshComp, const FName& InBoneName, const FName& InParentBoneName);
private:
	// 재정의 함수
	static class AWRCharacter* SpawnSlicedSkeletalMesh(const FString& InBPPath, const FVector& InLocation, const struct FRotator& InRotation, FVector InScale = FVector::OneVector);
	static class UProceduralMeshComponent* ConvertProcMeshFromSkelPart(USkeletalMeshComponent* InSkelMeshComp, int32 LodIndex, const TMap<uint32, uint32>& InTargetedMap);
	static int32 GetNewIndexForOldVertIndexFromSkinnedMesh(int32 MeshVertIndex, TMap<int32, int32>& MeshToSectionVertMap, USkeletalMeshComponent* InSkelMeshComp, const class FSkeletalMeshLODRenderData& InLOD, class FSkinWeightVertexBuffer& WeightBuffer, TArray<FVector>& Vertices, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<struct FProcMeshTangent>& Tangents);
	static void GetSectionFromSkeletalMesh(USkeletalMeshComponent * InSkelMeshComp, int32 LODIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, const TArray<uint32>& InIndices);
	
public:
	//static int32 SliceSkeletalMesh( USkeletalMeshComponent* InSkelMeshComp, FVector PlanePosition, FVector PlaneNormal, class UMaterialInterface* CapMaterial);
	static void SliceSkeletalMesh(USkeletalMeshComponent* InSkelMeshComp, FName InBoneName, FName InJointName, FVector PlanePosition, FVector PlaneNormal);
	static void CheckSkeletalMeshToSlice(AWRCharacter* InChar ,const TArray<struct FHitResult>& HitResults);
	static void CheckSkeletalMeshToSlice(AWRCharacter* InChar, const FHitResult& HitResults);
};

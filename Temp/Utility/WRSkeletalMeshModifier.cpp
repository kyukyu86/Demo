// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.

#include "WRSkeletalMeshModifier.h"

#include "Animation/Skeleton.h"
#include "SkeletalMeshRenderData.h"

#include "Engine/SkeletalMeshSocket.h"

#include "KismetProceduralMeshLibrary.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"


#include "Actor/Character/WRCharacter.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCharacterManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WREnpcTable.h"
#include "WRActorUtility.h"

#if UE_EDITOR
#include "Engine/Engine.h"
#endif // UE_EDITOR
#include "Manager/WRPathManager.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Components/SceneComponent.h"
#include "Manager/WRMeshModifyManager.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "Manager/WRSFXManager.h"
#include "Components/CapsuleComponent.h"
#include "SkinWeightVertexBuffer.h"
//#include "CombatSystem/Combat/WRCombatInfo.h"

WRSkeletalMeshModifier::WRSkeletalMeshModifier()
{
}

WRSkeletalMeshModifier::~WRSkeletalMeshModifier()
{
}



static void GetAllChildBone(FReferenceSkeleton* InRefSkel, int32 InBoneIndex, TArray<int32>& BoneList)
{
	BoneList.Add(InBoneIndex);
	TArray<FMeshBoneInfo> InMeshBoneInfos = InRefSkel->GetRefBoneInfo();

	for (int32 i = 0; i < InMeshBoneInfos.Num(); i++)
	{
		if (InMeshBoneInfos[i].ParentIndex == InBoneIndex && InMeshBoneInfos[i].Name.ToString().Contains(TEXT("weapon")) == false)
		{
			GetAllChildBone(InRefSkel, i, BoneList);
		}
	}
}

static TArray<int32> GetRelationBodies(USkeletalMeshComponent* InSkelMeshComp, TArray<int32> TargetBones)
{
	TArray<int32> OutBodyIndices;

	for (int32 BodyIndex = 0; BodyIndex < InSkelMeshComp->Bodies.Num(); BodyIndex++)
	{
		int32 TargetBoneIndex = InSkelMeshComp->Bodies[BodyIndex]->InstanceBoneIndex;

		if (TargetBones.Contains(TargetBoneIndex) == true)
			OutBodyIndices.Add(InSkelMeshComp->Bodies[BodyIndex]->InstanceBodyIndex);
	}

	return OutBodyIndices;
}


void WRSkeletalMeshModifier::BreakConstraintCustom(USkeletalMeshComponent* InSkelMeshComp, FVector Impulse, FVector HitLocation, FName InBoneName, FName InJointName)
{
	// you can enable/disable the instanced weights by calling
	int32 ConstraintIndex = InSkelMeshComp->FindConstraintIndex(InJointName);
	if (ConstraintIndex == INDEX_NONE || ConstraintIndex >= InSkelMeshComp->Constraints.Num())
	{
		return;
	}

	FConstraintInstance* Constraint = InSkelMeshComp->Constraints[ConstraintIndex];
	// If already broken, our job has already been done. Bail!
	if (Constraint->IsTerminated())
	{
		return;
	}

	UPhysicsAsset * const PhysicsAsset = InSkelMeshComp->GetPhysicsAsset();

	// Figure out if Body is fixed or not
	FBodyInstance* Body = InSkelMeshComp->GetBodyInstance(InBoneName);

	if (Body != NULL && !Body->IsInstanceSimulatingPhysics())
	{
		// Unfix body so it can be broken.
		Body->SetInstanceSimulatePhysics(true);
	}

	// Break Constraint
	Constraint->TermConstraint();
	// Make sure child bodies and constraints are released and turned to physics.
	InSkelMeshComp->UpdateMeshForBrokenConstraints();
	// Add impulse to broken limb
	InSkelMeshComp->AddImpulseAtLocation(Impulse, HitLocation, InBoneName);
}

AWRCharacter * WRSkeletalMeshModifier::SpawnSlicedSkeletalMesh(const FString & InBPPath, const FVector & InLocation, const FRotator & InRotation, FVector InScale)
{
	UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();

	FString BPFullPath;

	if (FPackageName::DoesPackageExist(InBPPath) == true)
	{
		BPFullPath = InBPPath;
	}
	else
	{
		//FString CharacterBPPath = WRPathManager::Get()->GetPath(EWRPath::Blueprint_Character);
		FString CharacterBPPath = WRPathManager::Get()->GetPath(EWRPath::Character);

		BPFullPath = CharacterBPPath + InBPPath;
	}
	AWRCharacter* SpawnedCharacter = nullptr;
	SpawnedCharacter = WRActorSpawnUtility::Spawn<AWRCharacter>(GameInstance, BPFullPath, InLocation, InRotation, InScale);


	if (SpawnedCharacter == nullptr)
	{
		UE_LOG(CharacterManager, Display, TEXT("WRCharacterManager::Spawn - Spawn failed. Please check your Blueprint Path."));
		return nullptr;
	}
	//SpawnedCharacter->SetSpawnDisplay(false);
	//SpawnedCharacter->AddToRoot();
	//SpawnedCharacter->OnPostSpawn();
	SpawnedCharacter->CreateComponent<UWRActorComponentManageMaterial>(EWRCharacterComponent::ManageMaterial);
	return SpawnedCharacter;
}


FName WRSkeletalMeshModifier::CreateNewSocketName(const USkeleton& InSkeleton, const FName& InBoneName)
{
	FName OutSocketName = FName();

	int32 SocketCount = 0;
	for (int32 SocketIndex = 0; SocketIndex < InSkeleton.Sockets.Num(); SocketIndex++)
	{
		if (InSkeleton.Sockets[SocketIndex]->BoneName == InBoneName)
			SocketCount++;
	}

	FString NewSocketNameStr = InBoneName.ToString() + FString::FromInt(SocketCount);
	OutSocketName = FName(*NewSocketNameStr);

	return OutSocketName;
}

void WRSkeletalMeshModifier::CreateSkeletalSocket(USkeleton* InSkeleton, const FTransform& RelativeTransform, FName BoneName, FName NewSocketName)
{
	if (InSkeleton == nullptr)
		return;

	USkeletalMeshSocket* NewSocket = NewObject<USkeletalMeshSocket>(InSkeleton);
	check(NewSocket);

	NewSocket->BoneName = BoneName;
	NewSocket->SocketName = NewSocketName;
	NewSocket->RelativeLocation = RelativeTransform.GetLocation();
	NewSocket->RelativeRotation = RelativeTransform.Rotator();
	NewSocket->RelativeScale = RelativeTransform.GetScale3D();

	InSkeleton->Sockets.Add(NewSocket);
}

// Find the most dominant bone for each vertex
int32 WRSkeletalMeshModifier::GetDominantBoneIndex(const FSkelMeshSkinWeightInfo& SkinWeightVert, bool bHasExtraInfluence)
{
	uint8 MaxWeightBone = 0;
	uint8 MaxWeightWeight = 0;

	uint32 InfluenceNum = 0;

	if (bHasExtraInfluence == true)
	{
		InfluenceNum = MAX_TOTAL_INFLUENCES;
	}
	else
	{
		InfluenceNum = MAX_INFLUENCES_PER_STREAM;
	}

	for (uint32 i = 0; i < InfluenceNum; i++)
	{
		if (SkinWeightVert.Weights[i] > MaxWeightWeight)
		{
			MaxWeightWeight = SkinWeightVert.Weights[i];
			MaxWeightBone = SkinWeightVert.Bones[i];
		}
	}
	return MaxWeightBone;
}

bool WRSkeletalMeshModifier::IsInsideOrOnInstance(const FBodyInstance* InBaseBodyInstance, const FTransform& InBaseBoneTransform, const FVector& InTargetLocation)
{
	FBox InBox = InBaseBodyInstance->BodySetup->AggGeom.CalcAABB(InBaseBoneTransform);

	return InBox.IsInsideOrOn(InTargetLocation);
}

FName WRSkeletalMeshModifier::GetClosetBodyInstanceBone(const USkeletalMeshComponent& InSkelMeshComp, const FVector& InHitLocation)
{
	uint32 BodysCount = InSkelMeshComp.Bodies.Num();

	for (uint32 BodyIndex = 0; BodyIndex < BodysCount; BodyIndex++)
	{
		const FBodyInstance* InBodyInstance = InSkelMeshComp.Bodies[BodyIndex];
		if (IsInsideOrOnInstance(InBodyInstance, InSkelMeshComp.GetBoneTransform(InBodyInstance->InstanceBoneIndex), InHitLocation) == true)
		{
			FName OutBoneName = InSkelMeshComp.GetBoneName(InBodyInstance->InstanceBoneIndex);
			return OutBoneName;
		}
	}
	return FName(TEXT("None"));
}

int32 WRSkeletalMeshModifier::GetClosetBodyInstanceIndex(const USkeletalMeshComponent & InSkelMeshComp, const FVector & InHitLocation)
{
	uint32 BodysCount = InSkelMeshComp.Bodies.Num();

	for (uint32 BodyIndex = 0; BodyIndex < BodysCount; BodyIndex++)
	{
		const FBodyInstance* InBodyInstance = InSkelMeshComp.Bodies[BodyIndex];
		if (IsInsideOrOnInstance(InBodyInstance, InSkelMeshComp.GetBoneTransform(InBodyInstance->InstanceBoneIndex), InHitLocation) == true)
		{
			return InBodyInstance->InstanceBodyIndex;
		}
	}
	return INDEX_NONE;
}

FName WRSkeletalMeshModifier::GetJointNameForCustomConstraint(const USkeletalMeshComponent & InSkelMeshComp, const FName & InBoneName, const FName & InParentBoneName)
{
	FName OutJointName = TEXT("None");
	TArray<FConstraintInstance*> InConstraints = InSkelMeshComp.Constraints;

	for (int32 ConstrantIndex = 0; ConstrantIndex < InConstraints.Num(); ConstrantIndex++)
	{
		FConstraintInstance* InConstrantInst = InConstraints[ConstrantIndex];

		if (InConstrantInst->ConstraintBone1 == InParentBoneName && InConstrantInst->ConstraintBone2 == InBoneName)
		{
			OutJointName = InConstrantInst->JointName;
			break;
		}
	}

	return OutJointName;
}

TArray<uint8> WRSkeletalMeshModifier::GetBoneListBelongToBody(const USkeletalMeshComponent& InSkelMeshComp, const FName& InBoneName, int32& ControllingParentBoneIndex)
{
	TArray<uint8> OutBoneList;

	FBodyInstance* InBodyInstance = InSkelMeshComp.GetBodyInstance(InSkelMeshComp.GetParentBone(InBoneName));

	if (InBodyInstance == nullptr)
		return OutBoneList;

	const TArray<FMeshBoneInfo>& InRawNameToIndexMap = InSkelMeshComp.SkeletalMesh->RefSkeleton.GetRefBoneInfo();

	uint32 BaseBoneIndex = InSkelMeshComp.GetBoneIndex(InSkelMeshComp.GetParentBone(InBoneName));

	int32 ControllingBodyIndex = InSkelMeshComp.GetPhysicsAsset()->FindControllingBodyIndex(InSkelMeshComp.SkeletalMesh, BaseBoneIndex);

	if (InSkelMeshComp.Bodies.IsValidIndex(ControllingBodyIndex))
	{
		int32 ControllingBoneIndex = InSkelMeshComp.Bodies[ControllingBodyIndex]->InstanceBoneIndex;
		FName ParentBoneName = InSkelMeshComp.GetParentBone(InSkelMeshComp.GetBoneName(ControllingBoneIndex));

		ControllingParentBoneIndex = InSkelMeshComp.GetBoneIndex(ParentBoneName);
	}

	int32 BoneCounts = InRawNameToIndexMap.Num();
	for (int32 InBoneIndex = 0; InBoneIndex < BoneCounts; InBoneIndex++)
	{
		if (InSkelMeshComp.GetPhysicsAsset()->FindControllingBodyIndex(InSkelMeshComp.SkeletalMesh, InBoneIndex) == ControllingBodyIndex)
		{
			OutBoneList.Add(InBoneIndex);
			continue;
		}
	}

	return OutBoneList;
}

int32 WRSkeletalMeshModifier::GetNewIndexForOldVertIndexFromSkinnedMesh(int32 MeshVertIndex, TMap<int32, int32>& MeshToSectionVertMap, USkeletalMeshComponent* InSkelMeshComp, const FSkeletalMeshLODRenderData& InLOD, FSkinWeightVertexBuffer& WeightBuffer, TArray<FVector>& Vertices, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents)
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

void WRSkeletalMeshModifier::GetSectionFromSkeletalMesh(USkeletalMeshComponent * InSkelMeshComp, int32 LODIndex, TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<struct FProcMeshTangent>& Tangents, const TArray<uint32>& InIndices)
{
	if (InSkelMeshComp == nullptr)
		return;

	USkeletalMesh * InSkeletalMesh = InSkelMeshComp->SkeletalMesh;

	if (InSkeletalMesh == nullptr)
		return;

	if (InSkeletalMesh->GetResourceForRendering() != nullptr &&
		InSkeletalMesh->GetResourceForRendering()->LODRenderData.IsValidIndex(LODIndex))
	{
		const FSkeletalMeshLODRenderData& InLOD = InSkeletalMesh->GetResourceForRendering()->LODRenderData[LODIndex];

		FSkinWeightVertexBuffer* WeightBuffer = InSkelMeshComp->GetSkinWeightBuffer(LODIndex);

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

		uint32 MaxIndiceCount = InIndices.Num();

		for (uint32 i = 0; i < MaxIndiceCount; i++)
		{
			uint32 MeshVertIndex = InIndices[i];

			int32 SectionVertIndex = GetNewIndexForOldVertIndexFromSkinnedMesh(MeshVertIndex, MeshToSectionVertMap, InSkelMeshComp, InLOD, *WeightBuffer, Vertices, Normals, UVs, Tangents);

			// Add to index buffer
			Triangles.Add(SectionVertIndex);
		}
	}
}

UProceduralMeshComponent * WRSkeletalMeshModifier::ConvertProcMeshFromSkelPart(USkeletalMeshComponent * InSkelMeshComp, int32 LodIndex, const TMap<uint32, uint32>& InTargetedMap)
{
	if (InSkelMeshComp == nullptr)
		return nullptr;

	UProceduralMeshComponent* NewProcMesh = NewObject<UProceduralMeshComponent>(InSkelMeshComp->GetOuter());

	if (NewProcMesh == nullptr)
		return nullptr;

	USkeletalMesh * InSkeletalMesh = InSkelMeshComp->SkeletalMesh;

	if (InSkeletalMesh == nullptr)
		return nullptr;

	FMultiSizeIndexContainerData CurrentMultiSizeIndexContainerData;

	CurrentMultiSizeIndexContainerData.DataTypeSize = InSkeletalMesh->GetResourceForRendering()->LODRenderData[LodIndex].MultiSizeIndexContainer.GetDataTypeSize();
	InSkeletalMesh->GetResourceForRendering()->LODRenderData[LodIndex].MultiSizeIndexContainer.GetIndexBuffer(CurrentMultiSizeIndexContainerData.Indices);
	//uint32 MaxIndiceCount = CurrentMultiSizeIndexContainerData.Indices.Num();

	uint32 NumSections = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[LodIndex].RenderSections.Num();

	for (uint32 SectionIndex = 0; SectionIndex < NumSections; SectionIndex++)
	{
		const FSkelMeshRenderSection& InSections = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[LodIndex].RenderSections[SectionIndex];

		const uint32 OnePastLastIndex = InSections.BaseIndex + InSections.NumTriangles * 3;

		TArray<uint32> InTargetedIndices;

		InTargetedIndices.Empty();

		//for (uint32 inIndex = 0; inIndex < MaxIndiceCount; inIndex += 3)
		for (uint32 inIndex = InSections.BaseIndex; inIndex < OnePastLastIndex; inIndex += 3)
		{
			uint32* InIndices[3];
			InIndices[0] = &CurrentMultiSizeIndexContainerData.Indices[inIndex];
			InIndices[1] = &CurrentMultiSizeIndexContainerData.Indices[inIndex + 1];
			InIndices[2] = &CurrentMultiSizeIndexContainerData.Indices[inIndex + 2];

			if (InIndices[0] != nullptr&&InIndices[1] != nullptr&&InIndices[2] != nullptr)
			{
				if (InTargetedMap.Find(*InIndices[0]) != nullptr &&InTargetedMap.Find(*InIndices[1]) != nullptr &&InTargetedMap.Find(*InIndices[2]) != nullptr)
				{
					for (uint32* InIndice : InIndices)
						InTargetedIndices.Add(*InIndice);
				}
			}
		}

		check(InTargetedIndices.Num() % 3 == 0);

		// create Only One Section for Test
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FVector2D> UVs1;
		TArray<FVector2D> UVs2;
		TArray<FVector2D> UVs3;
		TArray<FProcMeshTangent> Tangents;

		GetSectionFromSkeletalMesh(InSkelMeshComp, LodIndex, Vertices, Triangles, Normals, UVs, Tangents, InTargetedIndices);

		TArray<FLinearColor> DummyColors;

		NewProcMesh->CreateMeshSection_LinearColor(SectionIndex, Vertices, Triangles, Normals, UVs, UVs1, UVs2, UVs3, DummyColors, Tangents, false);
	}

	NewProcMesh->ClearCollisionConvexMeshes();

	if (InSkelMeshComp->BodySetup != nullptr)
	{
		// Iterate over all convex hulls on skeletal mesh..
		const int32 NumConvex = InSkelMeshComp->BodySetup->AggGeom.ConvexElems.Num();
		for (int ConvexIndex = 0; ConvexIndex < NumConvex; ConvexIndex++)
		{
			// Copy convex verts to ProcMesh
			FKConvexElem& MeshConvex = InSkelMeshComp->BodySetup->AggGeom.ConvexElems[ConvexIndex];
			NewProcMesh->AddCollisionConvexMesh(MeshConvex.VertexData);
		}
	}

	for (int32 MatIndex = 0; MatIndex < InSkelMeshComp->GetNumMaterials(); MatIndex++)
	{
		NewProcMesh->SetMaterial(MatIndex, InSkelMeshComp->GetMaterial(MatIndex));
	}

	NewProcMesh->SetWorldTransform(InSkelMeshComp->GetComponentTransform());


	NewProcMesh->RegisterComponent();


	return NewProcMesh;
}

//int32 WRSkeletalMeshModifier::SliceSkeletalMesh(USkeletalMeshComponent * InSkelMeshComp, FVector PlanePosition, FVector PlaneNormal, UMaterialInterface * CapMaterial)
//{
//	if (InSkelMeshComp != nullptr && InSkelMeshComp->SkeletalMesh != nullptr)
//	{
//		USkeleton* InSkeleton = InSkelMeshComp->SkeletalMesh->Skeleton;
//
//		if (InSkeleton == nullptr)
//			return INDEX_NONE;
//
//		FName BaseBoneName = GetClosetBodyInstanceBone(*InSkelMeshComp, PlanePosition);
//		if (BaseBoneName == FName(TEXT("None")) || BaseBoneName.ToString().Contains(TEXT("cloth")) || BaseBoneName.ToString().Contains(TEXT("weapon")))
//			return INDEX_NONE;
//
//		FName ParentBoneName = FName();
//		int32 BaseBoneindex = InSkelMeshComp->GetBoneIndex(BaseBoneName);
//		int32 ParentBoneIndex = INDEX_NONE;
//
//
//		// Initialize OverrideWeightInfos
//		const FSkinWeightVertexBuffer& InWeightsBuffer = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[0].SkinWeightVertexBuffer;
//
//		TArray< FSkelMeshSkinWeightInfo> OverrideWeightInfos;
//		OverrideWeightInfos.Empty(InWeightsBuffer.GetNumVertices());
//
//		for (uint32 SkinBufferIndex = 0; SkinBufferIndex < InWeightsBuffer.GetNumVertices(); SkinBufferIndex++)
//		{
//			const TSkinWeightInfo<true>* InWeight = InWeightsBuffer.GetSkinWeightPtr<true>(SkinBufferIndex);
//
//			int32 TargetSectionIndex, TargetVertexBelongInSection = 0;
//			InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[0].GetSectionFromVertexIndex(SkinBufferIndex, TargetSectionIndex, TargetVertexBelongInSection);
//
//			const FSkelMeshRenderSection& Section = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[0].RenderSections[TargetSectionIndex];
//
//			FSkelMeshSkinWeightInfo TestWeightInfo;
//			for (int32 i = 0; i < MAX_TOTAL_INFLUENCES; i++)
//			{
//				TestWeightInfo.Bones[i] = 0;
//				TestWeightInfo.Weights[i] = 0;
//
//				const int32 MeshBoneIndex = Section.BoneMap[InWeight->InfluenceBones[i]];
//
//				TestWeightInfo.Bones[i] = MeshBoneIndex;
//				TestWeightInfo.Weights[i] = InWeight->InfluenceWeights[i];
//			}
//			OverrideWeightInfos.Add(TestWeightInfo);
//		}
//
//		// 바디인스턴스에 속한 모든 본인덱스 받아오기
//		TArray<uint8> TargetBones = GetBoneListBelongToBody(*InSkelMeshComp, BaseBoneName, ParentBoneIndex);
//
//		ParentBoneName = InSkelMeshComp->GetBoneName(ParentBoneIndex);
//
//		FTransform InBoneTransform = InSkelMeshComp->GetBoneTransform(BaseBoneindex);
//		FTransform InParentBoneTransform = InSkelMeshComp->GetBoneTransform(ParentBoneIndex);
//
//		if (InBoneTransform.IsValid() == false)
//			return INDEX_NONE;
//		if (InParentBoneTransform.IsValid() == false)
//			return INDEX_NONE;
//
//		TMap<uint32, uint32> InTargetedMap;
//		TArray<FColor> VertexColors;
//		TArray<uint32> InTargetedWeightIndex;
//
//		uint32 OverrideWeightInfoCounts = OverrideWeightInfos.Num();
//
//		for (uint32 i = 0; i < OverrideWeightInfoCounts; i++)
//		{
//			VertexColors.Add(InSkelMeshComp->GetVertexColor(i));
//
//			int32 TargetSectionIndex, TargetVertexBelongInSection = 0;
//
//			InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[0].GetSectionFromVertexIndex(i, TargetSectionIndex, TargetVertexBelongInSection);
//
//			uint32 BelongBoneCounts = TargetBones.Num();
//
//			for (uint32 BelongBoneIndex = 0; BelongBoneIndex < BelongBoneCounts; BelongBoneIndex++)
//			{
//				const int32& BelongIndex = TargetBones[BelongBoneIndex];
//				if (GetDominantBoneIndex(OverrideWeightInfos[i]) == BelongIndex)
//				{
//					InTargetedWeightIndex.Add(i);
//					InTargetedMap.Add(i, TargetSectionIndex);
//
//					// SkinWeight Override용 세팅하는 부분, 현재 비활성화 추후 SkeletalMesh 직접 Slice시에 사용가능성 있음
//					for (uint32 InfluenceCount = 0; InfluenceCount < MAX_TOTAL_INFLUENCES; InfluenceCount++)
//					{
//						if (OverrideWeightInfos[i].Weights[InfluenceCount] > 0)
//						{
//							OverrideWeightInfos[i].Bones[InfluenceCount] = ParentBoneIndex;
//						}
//					}
//					break;
//				}
//			}
//		}
//
//		TArray<FVector> TargetPositions;
//		TArray<FMatrix> TargetMat;
//		InSkelMeshComp->CacheRefToLocalMatrices(TargetMat);
//
//		InSkelMeshComp->ComputeSkinnedPositions(InSkelMeshComp, TargetPositions, TargetMat, InSkelMeshComp->SkeletalMesh->GetResourceForRendering()->LODRenderData[0], InSkelMeshComp->SkeletalMesh->GetResourceForRendering()->LODRenderData[0].SkinWeightVertexBuffer);
//		const FTransform ToWorld = InSkelMeshComp->GetComponentTransform();
//		FVector WorldLocation = ToWorld.GetLocation();
//
//		uint32 InTargetedWeightIndexNum = InTargetedWeightIndex.Num();
//		for (uint32 inIndex = 0; inIndex < InTargetedWeightIndexNum; inIndex++)
//		{
//			uint32 VertIndex = InTargetedWeightIndex[inIndex];
//
//			VertexColors[VertIndex].A = 0;
//
//			/*FVector TestVec = TargetPositions[VertIndex];
//
//			TestVec = WorldLocation + ToWorld.TransformVector(TestVec);
//
//			UKismetSystemLibrary::DrawDebugPoint(InSkelMeshComp->GetWorld(), TestVec, 10.f, FLinearColor::Red, 5.f);*/
//		}
//
//		UProceduralMeshComponent* BasePartMesh = ConvertProcMeshFromSkelPart(InSkelMeshComp, LODINDEX_BASE, InTargetedMap);
//		UProceduralMeshComponent* AnotherHalfPartMesh = nullptr;
//
//		if (BasePartMesh == nullptr)
//			nullptr;
//
//		InSkelMeshComp->SetVertexColorOverride(LODINDEX_BASE, VertexColors);
//		InSkelMeshComp->SetSkinWeightOverride(LODINDEX_BASE, OverrideWeightInfos);
//
//		FName NewBaseSocketName = CreateNewSocketName(*InSkeleton, BaseBoneName);
//		FName NewParentSocketName = CreateNewSocketName(*InSkeleton, ParentBoneName);
//
//		CreateSkeletalSocket(InSkeleton, InBoneTransform, BaseBoneName, NewParentSocketName);
//		CreateSkeletalSocket(InSkeleton, InParentBoneTransform, ParentBoneName, NewBaseSocketName);
//
//		EProcMeshSliceCapOption InCapOption;
//
//		if (CapMaterial == nullptr)
//		{
//			InCapOption = EProcMeshSliceCapOption::NoCap;
//		}
//		else
//		{
//			InCapOption = EProcMeshSliceCapOption::CreateNewSectionForCap;
//		}
//
//		UKismetProceduralMeshLibrary::SliceProceduralMesh(BasePartMesh, PlanePosition, PlaneNormal, true, AnotherHalfPartMesh, InCapOption, CapMaterial);
//
//		InSkelMeshComp->BreakConstraint(PlaneNormal*0.f, PlanePosition, BaseBoneName);
//
//		FPlane SlicePlane = FPlane(PlanePosition, PlaneNormal);
//
//		float PartDist = SlicePlane.PlaneDot(InParentBoneTransform.GetLocation());
//
//		if (PartDist > 0.f)
//		{
//			BasePartMesh->AttachToComponent(InSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewParentSocketName);
//			AnotherHalfPartMesh->AttachToComponent(InSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewBaseSocketName);
//		}
//		else
//		{
//			BasePartMesh->AttachToComponent(InSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewBaseSocketName);
//			AnotherHalfPartMesh->AttachToComponent(InSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewParentSocketName);
//		}
//
//		//InSkelMeshComp->SetAllBodiesBelowPhysicsBlendWeight(BaseBoneName, 1.f, false, true);
//		InSkelMeshComp->SetSimulatePhysics(true);
//		return InSkelMeshComp->GetPhysicsAsset()->FindControllingBodyIndex(InSkelMeshComp->SkeletalMesh, BaseBoneindex);
//	}
//	return INDEX_NONE;
//}

void WRSkeletalMeshModifier::SliceSkeletalMesh(USkeletalMeshComponent * InSkelMeshComp, FName InBoneName, FName InJointName, FVector PlanePosition, FVector PlaneNormal)
{
	if (InSkelMeshComp != nullptr && InSkelMeshComp->SkeletalMesh != nullptr)
	{
		USkeleton* InSkeleton = InSkelMeshComp->SkeletalMesh->Skeleton;

		int32 CurrentLODIndex = InSkelMeshComp->PredictedLODLevel;

		if (InSkeleton == nullptr)
			return;

		FName BaseBoneName = InBoneName;

		if (BaseBoneName == FName(TEXT("None")) || BaseBoneName.ToString().Contains(TEXT("cloth")) || BaseBoneName.ToString().Contains(TEXT("weapon")))
			return;

		int32 BaseBoneindex = InSkelMeshComp->GetBoneIndex(BaseBoneName);

		FName ParentBoneName = InSkelMeshComp->GetParentBone(BaseBoneName);
		int32 ParentBoneIndex = InSkelMeshComp->GetBoneIndex(ParentBoneName);

		// Initialize OverrideWeightInfos
#if UE_4_25
		FSkinWeightVertexBuffer* WeightsBuffer = InSkelMeshComp->GetSkinWeightBuffer(CurrentLODIndex);

		TArray< FSkelMeshSkinWeightInfo> OverrideWeightInfos;
		OverrideWeightInfos.Empty(WeightsBuffer->GetNumVertices());
#else // UE_4_25
		const FSkinWeightVertexBuffer& InWeightsBuffer = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].SkinWeightVertexBuffer;

		TArray< FSkelMeshSkinWeightInfo> OverrideWeightInfos;
		OverrideWeightInfos.Empty(InWeightsBuffer->GetNumVertices());
#endif // UE_4_25



#if UE_4_25

		
#else // UE_4_25
		if (InWeightsBuffer.HasExtraBoneInfluences() == true)
		{
			for (uint32 SkinBufferIndex = 0; SkinBufferIndex < InWeightsBuffer.GetNumVertices(); SkinBufferIndex++)
			{

				const TSkinWeightInfo<true>* InWeight = InWeightsBuffer.GetSkinWeightPtr<true>(SkinBufferIndex);

				int32 TargetSectionIndex, TargetVertexBelongInSection = 0;

				InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].GetSectionFromVertexIndex(SkinBufferIndex, TargetSectionIndex, TargetVertexBelongInSection);

				const FSkelMeshRenderSection& Section = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].RenderSections[TargetSectionIndex];

				FSkelMeshSkinWeightInfo InWeightInfo;
				uint32 NumInfluence = InWeight->NumInfluences;
				for (uint32 i = 0; i < NumInfluence; i++)
				{
					InWeightInfo.Bones[i] = 0;
					InWeightInfo.Weights[i] = 0;

					const int32 MeshBoneIndex = Section.BoneMap[InWeight->InfluenceBones[i]];

					InWeightInfo.Bones[i] = MeshBoneIndex;
					InWeightInfo.Weights[i] = InWeight->InfluenceWeights[i];
				}
				OverrideWeightInfos.Add(InWeightInfo);
			}
		}
		else
		{
			for (uint32 SkinBufferIndex = 0; SkinBufferIndex < InWeightsBuffer.GetNumVertices(); SkinBufferIndex++)
			{

				const TSkinWeightInfo<false>* InWeight = InWeightsBuffer.GetSkinWeightPtr<false>(SkinBufferIndex);

				int32 TargetSectionIndex, TargetVertexBelongInSection = 0;
				InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].GetSectionFromVertexIndex(SkinBufferIndex, TargetSectionIndex, TargetVertexBelongInSection);

				const FSkelMeshRenderSection& Section = InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].RenderSections[TargetSectionIndex];

				FSkelMeshSkinWeightInfo InWeightInfo;
				uint32 NumInfluence = InWeight->NumInfluences;
				for (uint32 i = 0; i < NumInfluence; i++)
				{
					InWeightInfo.Bones[i] = 0;
					InWeightInfo.Weights[i] = 0;

					const int32 MeshBoneIndex = Section.BoneMap[InWeight->InfluenceBones[i]];

					InWeightInfo.Bones[i] = MeshBoneIndex;
					InWeightInfo.Weights[i] = InWeight->InfluenceWeights[i];
				}
				OverrideWeightInfos.Add(InWeightInfo);
			}
		}
#endif // UE_4_25

		// 바디인스턴스에 속한 모든 본인덱스 받아오기
		TArray<int32> TargetBones;
		GetAllChildBone(&InSkelMeshComp->SkeletalMesh->RefSkeleton, BaseBoneindex, TargetBones);

		TArray<int32> BaseBodyIndices;

		ParentBoneName = InSkelMeshComp->GetBoneName(ParentBoneIndex);

		FTransform InBoneTransform = InSkelMeshComp->GetBoneTransform(BaseBoneindex);
		FTransform InParentBoneTransform = InSkelMeshComp->GetBoneTransform(ParentBoneIndex);

		if (InBoneTransform.IsValid() == false)
			return;
		if (InParentBoneTransform.IsValid() == false)
			return;

		TMap<uint32, uint32> InTargetedMap;
		TArray<FColor> BaseVertexColors;
		TArray<FColor> AnotherVertexColors;
		TArray<uint32> InTargetedWeightIndex;

		uint32 OverrideWeightInfoCounts = OverrideWeightInfos.Num();

		for (uint32 i = 0; i < OverrideWeightInfoCounts; i++)
		{
			BaseVertexColors.Add(InSkelMeshComp->GetVertexColor(i));
			AnotherVertexColors.Add(InSkelMeshComp->GetVertexColor(i));

			int32 TargetSectionIndex, TargetVertexBelongInSection = 0;

			InSkelMeshComp->GetSkeletalMeshRenderData()->LODRenderData[CurrentLODIndex].GetSectionFromVertexIndex(i, TargetSectionIndex, TargetVertexBelongInSection);

			uint32 BelongBoneCounts = TargetBones.Num();

			for (uint32 BelongBoneIndex = 0; BelongBoneIndex < BelongBoneCounts; BelongBoneIndex++)
			{
				const int32& BelongIndex = TargetBones[BelongBoneIndex];
#if UE_4_25
				if (GetDominantBoneIndex(OverrideWeightInfos[i], WeightsBuffer->GetBoneInfluenceType() == GPUSkinBoneInfluenceType::ExtraBoneInfluence) == BelongIndex)
#else // UE_4_25
				if (GetDominantBoneIndex(OverrideWeightInfos[i], InWeightsBuffer.HasExtraBoneInfluences()) == BelongIndex)
#endif // UE_4_25
				{
					InTargetedMap.Add(i, TargetSectionIndex);
					break;
				}
			}
		}

		FPlane SlicePlane = FPlane(PlanePosition, PlaneNormal);

		TArray<FVector> TargetPositions;
		TArray<FMatrix> TargetMat;
		InSkelMeshComp->CacheRefToLocalMatrices(TargetMat);

		InSkelMeshComp->ComputeSkinnedPositions(InSkelMeshComp, TargetPositions, TargetMat, InSkelMeshComp->SkeletalMesh->GetResourceForRendering()->LODRenderData[CurrentLODIndex], InSkelMeshComp->SkeletalMesh->GetResourceForRendering()->LODRenderData[CurrentLODIndex].SkinWeightVertexBuffer);
		bool ExceptionSlice = false;
		for (uint32 i = 0; i < OverrideWeightInfoCounts; i++)
		{
			FVector PointVec = TargetPositions[i];

			const FTransform ToWorld = InSkelMeshComp->GetComponentToWorld();
			FVector WorldLocation = ToWorld.GetLocation();
			PointVec = WorldLocation + ToWorld.TransformVector(PointVec);

			float PointDist = SlicePlane.PlaneDot(PointVec);

			if (InTargetedMap.Find(i) != nullptr &&  PointDist >= 2.f)
			{
				BaseVertexColors[i].A = 0;
				ExceptionSlice = true;
			}
			else
			{
				AnotherVertexColors[i].A = 0;
				ExceptionSlice = true;
			}
		}

		if (ExceptionSlice == false)
			return;

		AWRCharacter* OwnerChar = Cast<AWRCharacter>(InSkelMeshComp->GetOwner());

		if (OwnerChar == nullptr)
			return;

		FWRNPCBaseTable* OutTable = WRTableManager::Get()->FindRow<FWREnpcTable>(OwnerChar->GetCharacterTableID());
		if (OutTable == nullptr)
			return;

		FWRCharacterSpawnParams SpawnParam(0, OwnerChar->GetCharacterTableID(), OutTable->BlueprintID, OwnerChar->GetActorLocation(), OwnerChar->GetActorRotation(), FVector::OneVector);

		AWRCharacter* SpawnChar = SpawnSlicedSkeletalMesh(SpawnParam.BPPath, SpawnParam.Location, SpawnParam.Rotation, FVector::OneVector);//WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam);

		if (SpawnChar == nullptr)
			return;

		SpawnChar->SetActorTransform(OwnerChar->GetActorTransform());



		InSkelMeshComp->SetVertexColorOverride(CurrentLODIndex, BaseVertexColors);

		USkeletalMeshComponent * DuplicatedSkelMeshComp = SpawnChar->GetMesh();

		if (DuplicatedSkelMeshComp == nullptr)
			return;

		DuplicatedSkelMeshComp->SetVertexColorOverride(CurrentLODIndex, AnotherVertexColors);

		InSkelMeshComp->SetForcedLOD(CurrentLODIndex + 1);
		DuplicatedSkelMeshComp->SetForcedLOD(CurrentLODIndex + 1);

		DuplicatedSkelMeshComp->SetSimulatePhysics(true);

		AWRCharacterMine* Player = WRCharacterManager::Get()->GetMyCharacter();

		if (Player == nullptr)
			return;

		UPrimitiveComponent * DuplicatePrimComp = Cast<UPrimitiveComponent>(SpawnChar->GetRootComponent());

		if (DuplicatePrimComp == nullptr)
			return;

		DuplicatePrimComp->SetCollisionResponseToChannel(Player->GetRootComponent()->GetCollisionObjectType(), ECollisionResponse::ECR_Ignore);
		DuplicatePrimComp->SetCollisionResponseToChannel(OwnerChar->GetRootComponent()->GetCollisionObjectType(), ECollisionResponse::ECR_Ignore);

		FVector DirectionWeight;
		float SliceDirectionWeight = WRConsoleManager::Get()->GetFloat(EWRConsole::Slice_DirectionWeight);
		if (BaseBoneName.ToString().Contains(TEXT("_r")))
		{
			DirectionWeight = FVector::LeftVector * SliceDirectionWeight;
		}
		else
		{
			DirectionWeight = FVector::RightVector * SliceDirectionWeight;
		}

		FVector PhysicsDirection = OwnerChar->GetPhysicsDirection() + DirectionWeight;
		PhysicsDirection = PhysicsDirection.GetSafeNormal();

		SpawnChar->SetPhysicsDirection(PhysicsDirection);

		BreakConstraintCustom(DuplicatedSkelMeshComp, 30000.f*(SpawnChar->GetPhysicsDirection()), InBoneTransform.GetLocation(), BaseBoneName, InJointName);
		WRMeshModifyManager::Get()->AddSlicedCharacters(SpawnChar);

		UParticleSystemComponent* OwnerParticleComp = Cast<UParticleSystemComponent>(OwnerChar->GetComponentByClass(UParticleSystemComponent::StaticClass()));
		UParticleSystemComponent* SpawnParticleComp = Cast<UParticleSystemComponent>(SpawnChar->GetComponentByClass(UParticleSystemComponent::StaticClass()));

		if (OwnerParticleComp != nullptr)
		{
			OwnerParticleComp->SetVisibility(false);
			OwnerParticleComp->SetActive(false);
		}

		if (SpawnParticleComp != nullptr)
		{
			OwnerParticleComp->SetVisibility(false);
			SpawnParticleComp->SetActive(false);
		}

		OwnerChar->SetIsSliced(true);
		SpawnChar->SetIsSliced(true);
		// [2019-12-20 | LJH] 사망후 거리벌리기위한 Collision 비활성화 를 위한 임시코드 추후 룰정해지고 삭제되어야 함
		TSet<UActorComponent*> SpawnCompSet = SpawnChar->GetComponents();

		for (UActorComponent* InComp : SpawnCompSet)
		{
			if (InComp == nullptr)
				continue;

			FString CompName = InComp->GetName();
			if (CompName == TEXT("collision"))
			{
				UCapsuleComponent* InCapsuleComp = Cast<UCapsuleComponent>(InComp);
				if (InCapsuleComp != nullptr)
					InCapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}

		//SpawnChar->DetermineFSMSignal(EWRFSM::Die);
		WRMeshModifyManager::Get()->RemoveSlicedCharacters(SpawnChar);
		WRCharacterManager::Get()->Remove(SpawnChar, SpawnChar->GetRemoveDisplay());
		
		UMaterialInterface* InCapMaterial = OwnerChar->InCapMaterial;

		if (InCapMaterial != nullptr)
		{
			UProceduralMeshComponent* BasePartMesh = ConvertProcMeshFromSkelPart(InSkelMeshComp, CurrentLODIndex, InTargetedMap);
			UProceduralMeshComponent* AnotherHalfPartMesh = nullptr;
			if (BasePartMesh == nullptr)
				return;

			BasePartMesh->SetWorldLocation(InSkelMeshComp->GetComponentLocation());

			FName NewBaseSocketName = CreateNewSocketName(*InSkeleton, ParentBoneName);
			USkeleton* DuplicatedSkel = DuplicatedSkelMeshComp->SkeletalMesh->Skeleton;
			CreateSkeletalSocket(InSkeleton, InBoneTransform, BaseBoneName, NewBaseSocketName);
			CreateSkeletalSocket(DuplicatedSkel, InBoneTransform, BaseBoneName, NewBaseSocketName);

			UKismetProceduralMeshLibrary::SliceProceduralMesh(BasePartMesh, PlanePosition, PlaneNormal, true, AnotherHalfPartMesh, EProcMeshSliceCapOption::CreateNewSectionForCap, InCapMaterial);

			int32 BasePartSectionNum = BasePartMesh->GetNumSections();

			for (int32 i = 0; i < BasePartSectionNum - 1; i++)
			{
				BasePartMesh->ClearMeshSection(i);
			}

			if (AnotherHalfPartMesh != nullptr)
			{
				int32 AnotherPartSectionNum = AnotherHalfPartMesh->GetNumSections();

				for (int32 i = 0; i < AnotherPartSectionNum - 1; i++)
				{
					AnotherHalfPartMesh->ClearMeshSection(i);
				}
			}
			
			BasePartMesh->AttachToComponent(InSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewBaseSocketName);
			if (AnotherHalfPartMesh != nullptr)
				AnotherHalfPartMesh->AttachToComponent(DuplicatedSkelMeshComp, FAttachmentTransformRules::KeepWorldTransform, NewBaseSocketName);
		}
	}
}

#define FIRST_HITPOINT 0
//#define FIELD_BOSSTID 10000001
//#define FIELD_BOSSTID2 10000007
//#define FIELD_BOSSTID3 10000011
#define FIELD_MONSTER1 10000002
#define FIELD_MONSTER2 10000003
//#define MIDDLE_BOSSTID 0
#define SLICE_LERPALPHA 0.6f

void WRSkeletalMeshModifier::CheckSkeletalMeshToSlice(AWRCharacter* InChar, const TArray<FHitResult>& HitResults)
{

	if (InChar != nullptr && InChar->GetIsSliced() == false && HitResults.Num() > 0)
	{
		//if (InChar->GetCharacterTableID() == FIELD_BOSSTID || InChar->GetCharacterTableID() == FIELD_BOSSTID2 || InChar->GetCharacterTableID() == FIELD_BOSSTID3)
		if (InChar->GetCharacterTableID() == FIELD_MONSTER1 || InChar->GetCharacterTableID() == FIELD_MONSTER2)
		{

			FName calf_l = FName("calf_l");
			FName calf_r = FName("calf_r");
			FName lowerarm_l = FName("lowerarm_l");
			FName lowerarm_r = FName("lowerarm_r");

			TArray<FName> BoneList;
			BoneList.Add(calf_l);
			BoneList.Add(calf_r);
			BoneList.Add(lowerarm_l);
			BoneList.Add(lowerarm_r);

			FHitResult InHitResult;// = HitResults[FIRST_HITPOINT];

			bool CheckValidHit = false;

			for (int32 HitIndex = 0; HitIndex < HitResults.Num(); HitIndex++)
			{
				if (InChar->GetUniqueID() == HitResults[HitIndex].GetActor()->GetUniqueID())
				{
					InHitResult = HitResults[HitIndex];
					CheckValidHit = true;
					break;
				}
			}
			if (CheckValidHit == false)
				return;

			USkeletalMeshComponent* InSkelMeshComp = Cast<USkeletalMeshComponent>(InHitResult.GetComponent());

			if (InSkelMeshComp == nullptr)
				return;

			float ClosetDist = -1.f;
			int32 ClosetBoneIndex = 0;
			int32 ClosetBoneNameIndex = 0;

			for (int32 BoneNameIndex = 0; BoneNameIndex < BoneList.Num(); BoneNameIndex++)
			{
				int32 BoneIndex = InSkelMeshComp->GetBoneIndex(BoneList[BoneNameIndex]);

				if (BoneIndex == INDEX_NONE)
					continue;

				FTransform BoneTransform = InSkelMeshComp->GetBoneTransform(BoneIndex);

				float TempDist = FVector::Dist(InHitResult.ImpactPoint, BoneTransform.GetLocation());

				if (ClosetDist < 0.f)
				{
					ClosetDist = TempDist;
					ClosetBoneIndex = BoneIndex;
					ClosetBoneNameIndex = BoneNameIndex;
				}
				else if (ClosetDist > TempDist)
				{
					ClosetDist = TempDist;
					ClosetBoneIndex = BoneIndex;
					ClosetBoneNameIndex = BoneNameIndex;
				}
			}

			FName BoneName = InSkelMeshComp->GetBoneName(ClosetBoneIndex);
			FName ParentBoneName = InSkelMeshComp->GetParentBone(BoneName);

			int32 ParentBoneIndex = InSkelMeshComp->GetBoneIndex(ParentBoneName);
			FTransform BoneTransform = InSkelMeshComp->GetBoneTransform(ClosetBoneIndex);
			FTransform ParentBoneTransform = InSkelMeshComp->GetBoneTransform(ParentBoneIndex);
			FVector BoneRotatorVec = BoneTransform.Rotator().Vector();

			FName JointName = GetJointNameForCustomConstraint(*InSkelMeshComp, ParentBoneName, InSkelMeshComp->GetParentBone(ParentBoneName));

			FVector SliceLocation = FMath::Lerp(ParentBoneTransform.GetLocation(), BoneTransform.GetLocation(), SLICE_LERPALPHA);

			FVector NormalVec = (BoneTransform.GetLocation() - ParentBoneTransform.GetLocation()).GetSafeNormal();

			SliceSkeletalMesh(InSkelMeshComp, ParentBoneName, JointName, SliceLocation, NormalVec);
		}
	}
}

void WRSkeletalMeshModifier::CheckSkeletalMeshToSlice(AWRCharacter * InChar, const FHitResult & HitResults)
{
}

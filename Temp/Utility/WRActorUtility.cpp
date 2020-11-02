#include "WRActorUtility.h"

#include "Core/WRGameInstance.h"

#include "Components/MeshComponent.h"
#include <Components/SplineComponent.h>
#include <Components/CapsuleComponent.h>

#include "Materials/MaterialInstanceDynamic.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/EnvActor/SlicedCharacter/WRSlicedActor.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Actor/StaticMeshActor/WRStaticMeshProjectile.h"
#include "Actor/Vehicle/WRVehicle.h"

#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRConsoleManager.h"

#include "Table/WRSkillTable.h"
#include "Table/WRPlayerTable.h"

#include "ModuleManager.h"

#include <TimerManager.h>
#include <Engine/World.h>
#include <Kismet/KismetSystemLibrary.h>

#include "WREnumUtility.h"

#include "NavigationSystem.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include "SkeletalMeshMerge.h"


FVector WRActorUtility::GetDirection(class AActor* InFrom, class AActor* InTo)
{
	FVector Result = InTo->GetActorLocation() - InFrom->GetActorLocation();
	Result.Normalize();

	return Result;
}

FVector WRActorUtility::GetDirection(const FVector& InFrom, const FVector& InTo)
{
	FVector Result = InTo - InFrom;
	Result.Normalize();

	return Result;
}

FTransform WRActorUtility::GetSocketTransform(const class AWheeledVehicle* InVehicle, const FName& InSocketName, const bool InIsComponentSpace /*= false*/, const bool InIsPure /*= false*/)
{
	if (InVehicle == nullptr)
	{
		return FTransform::Identity;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InVehicle->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return FTransform::Identity;
	}

	if (SkeletalMeshComponent->DoesSocketExist(InSocketName) == false)
	{
		return InVehicle->GetActorTransform();
	}

	// [ 2019-9-23 : animal14 ] 액터에서 변경된 내용을 적용 (크기, 회전, 위치)
	if (InIsComponentSpace == false)
	{
		return SkeletalMeshComponent->GetSocketTransform(InSocketName);
	}

	FTransform Transform = SkeletalMeshComponent->GetSocketTransform(InSocketName, ERelativeTransformSpace::RTS_Component);

	// [ 2019-9-23 : animal14 ] BP에서 변경된 내용을 컴포넌트 스페이스에 적용 (위치)
	if (InIsPure == false)
	{
		Transform.AddToTranslation(SkeletalMeshComponent->GetRelativeTransform().GetLocation());
	}

	// [ 2019-9-23 : animal14 ] 순수하게 컴포넌트 스페이스만 사용
	Transform.AddToTranslation(InVehicle->GetActorLocation());

	return Transform;
}

FTransform WRActorUtility::GetSocketTransform(const class ACharacter* InCharacter, const FName& InSocketName, const bool InIsComponentSpace /*= false*/, const bool InIsPure /*= false*/)
{
	if (InCharacter == nullptr)
	{
		return FTransform::Identity;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return FTransform::Identity;
	}

	if (SkeletalMeshComponent->DoesSocketExist(InSocketName) == false)
	{
		return InCharacter->GetActorTransform();
	}

	// [ 2019-9-23 : animal14 ] 액터에서 변경된 내용을 적용 (크기, 회전, 위치)
	if (InIsComponentSpace == false)
	{
		return SkeletalMeshComponent->GetSocketTransform(InSocketName);
	}

	FTransform Transform = SkeletalMeshComponent->GetSocketTransform(InSocketName, ERelativeTransformSpace::RTS_Component);

	// [ 2019-9-23 : animal14 ] BP에서 변경된 내용을 컴포넌트 스페이스에 적용 (위치)
	if (InIsPure == false)
	{
		Transform.AddToTranslation(SkeletalMeshComponent->GetRelativeTransform().GetLocation());
	}

	// [ 2019-9-23 : animal14 ] 순수하게 컴포넌트 스페이스만 사용
	Transform.AddToTranslation(InCharacter->GetActorLocation());

	return Transform;
}

FTransform WRActorUtility::GetSocketTransform(const class AActor* InActor, const FName& InSocketName, const bool InIsComponentSpace /*= false*/, const bool InIsPure /*= false*/)
{
	if (InActor == nullptr)
	{
		return FTransform::Identity;
	}

	const ACharacter* Character = Cast<const ACharacter>(InActor);
	if (Character)
	{
		return WRActorUtility::GetSocketTransform(Character, InSocketName, InIsComponentSpace, InIsPure);
	}

	const AWheeledVehicle* Vehicle = Cast<const AWheeledVehicle>(InActor);
	if (Vehicle)
	{
		return WRActorUtility::GetSocketTransform(Vehicle, InSocketName, InIsComponentSpace, InIsPure);
	}

	return InActor->GetActorTransform();
}

FTransform WRActorUtility::GetSocketLocalTransform(const class ACharacter* InCharacter, const FName& InSocketName)
{
	if (InCharacter == nullptr)
	{
		return FTransform::Identity;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return FTransform::Identity;
	}

	if (SkeletalMeshComponent->DoesSocketExist(InSocketName) == false)
	{
		return InCharacter->GetActorTransform();
	}

	return SkeletalMeshComponent->GetSocketTransform(InSocketName, ERelativeTransformSpace::RTS_Actor);
}

FTransform WRActorUtility::GetBoneTransform(class AWheeledVehicle* InVehicle, const FName& InBoneName)
{
	if (InVehicle == nullptr)
	{
		return FTransform::Identity;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InVehicle->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return FTransform::Identity;
	}

	int32 Index = SkeletalMeshComponent->GetBoneIndex(InBoneName);
	if (Index < 0)
	{
		return InVehicle->GetActorTransform();
	}

	return SkeletalMeshComponent->GetBoneTransform(Index);
}

FTransform WRActorUtility::GetBoneTransform(class ACharacter* InCharacter, const FName& InBoneName)
{
	if (InCharacter == nullptr)
	{
		return FTransform::Identity;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return FTransform::Identity;
	}

	int32 Index = SkeletalMeshComponent->GetBoneIndex(InBoneName);
	if (Index < 0)
	{
		return InCharacter->GetActorTransform();
	}

	return SkeletalMeshComponent->GetBoneTransform(Index);
}

FTransform WRActorUtility::GetBoneTransform(class AActor* InActor, const FName& InBoneName)
{
	if (InActor == nullptr)
	{
		return FTransform::Identity;
	}

	ACharacter* Character = Cast<ACharacter>(InActor);
	if (Character)
	{
		return WRActorUtility::GetBoneTransform(Character, InBoneName);
	}

	AWheeledVehicle* Vehicle = Cast<AWheeledVehicle>(InActor);
	if (Vehicle)
	{
		return WRActorUtility::GetBoneTransform(Vehicle, InBoneName);
	}

	return InActor->GetActorTransform();
}

FVector WRActorUtility::GetSocketDirection(class ACharacter* InFrom, class ACharacter* InTo, const FName& InFromSocketName, const FName& InToSocketName)
{
	return WRActorUtility::GetDirection(WRActorUtility::GetSocketTransform(InFrom, InFromSocketName).GetLocation(), WRActorUtility::GetSocketTransform(InTo, InToSocketName).GetLocation());
}

FVector WRActorUtility::GetBoneDirection(class ACharacter* InFrom, class ACharacter* InTo, const FName& InFromBoneName, const FName& InToBoneName)
{
	return WRActorUtility::GetDirection(WRActorUtility::GetBoneTransform(InFrom, InFromBoneName).GetLocation(), WRActorUtility::GetBoneTransform(InTo, InToBoneName).GetLocation());
}

void WRActorUtility::IsRooted(class AActor* InActor)
{
	TArray<UActorComponent*> OutComponents;
	InActor->GetComponents(OutComponents);

	for (auto& OutComponent : OutComponents)
	{
		if (OutComponent->IsRooted())
		{
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(OutComponent->GetName());
			StringArgs.Add(InActor->GetName());

			FString Text = FString::Format(TEXT("{0} of {1} is rooted."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("!IsRooted()"));
		}
	}
}

FName WRActorUtility::GetNearestBoneName(class AWRCharacter* InCharacter, const FVector& InWorldLocation, const TArray<FString>& InIgnoreBoneName)
{
	if (InCharacter->IsValidLowLevel() == false)
		return NAME_None;

	FName NearestBoneName = NAME_None;
	float NearestDistance = 10000000.0f;

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	TArray<FName> BoneNames;
	SkeletalMeshComponent->GetBoneNames(BoneNames);
	for (int32 i = 0; i < BoneNames.Num(); ++i)
	{
		bool bContinue = false;
		for (int32 j = 0; j < InIgnoreBoneName.Num(); ++j)
		{
			if (BoneNames[i].ToString().ToLower().Contains(InIgnoreBoneName[j]) == true)
			{
				bContinue = true;
				break;
			}
		}
		
		if(bContinue == true)
			continue;

		FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(BoneNames[i]);
		float Distance = FVector::Dist(SocketLocation, InWorldLocation);
		if (Distance < NearestDistance)
		{
			NearestBoneName = BoneNames[i];
			NearestDistance = Distance;
		}
	}

	return NearestBoneName;
}

FName WRActorUtility::GetNearestSocketName(class AWRCharacter* InCharacter, const FVector& InWorldLocation)
{
	if (InCharacter->IsValidLowLevel() == false)
		return NAME_None;


	FName NearestSocketName = NAME_None;
	float NearestDistance = 10000000.0f;

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	TArray<FName> SocketsName = SkeletalMeshComponent->GetAllSocketNames();
	for (int32 i = 0; i < SocketsName.Num(); ++i)
	{
		FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketsName[i]);
		float Distance = FVector::Dist(SocketLocation, InWorldLocation);
		if (Distance < NearestDistance)
		{
			NearestSocketName = SocketsName[i];
			NearestDistance = Distance;
		}
	}

	return NearestSocketName;
}

FName WRActorUtility::GetNearestSocketNameWithTitleName(class AWRCharacter* InCharacter, const FVector& InWorldLocation, const FString& InTitleName, const EWRCollisionType InPart)
{
	if (InCharacter->IsValidLowLevel() == false)
		return NAME_None;


	FName NearestSocketName = NAME_None;
	float NearestDistance = 10000000.0f;

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	TArray<FName> SocketsName = SkeletalMeshComponent->GetAllSocketNames();
	for (int32 i = 0; i < SocketsName.Num(); ++i)
	{
		FString strSocketName = SocketsName[i].ToString();
		bool bFind = strSocketName.Contains(InTitleName);
		if (bFind)
		{
			FString strCollision = WREnumUtility::EnumToString("EWRCollisionType", InPart);
			bFind = strSocketName.Contains(strCollision);
			if (bFind)
			{
				FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketsName[i]);
				float Distance = FVector::Dist(SocketLocation, InWorldLocation);
				if (Distance < NearestDistance)
				{
					NearestSocketName = SocketsName[i];
					NearestDistance = Distance;
				}
			}
		}
	}

	return NearestSocketName;
}

bool WRActorUtility::IsOnNavigation(const UWorld* InWorld, const FVector& InLocation)
{
	if (InWorld == nullptr)
	{
		return false;
	}

	const UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(InWorld);
	if (NavigationSystem == nullptr)
	{
		return false;
	}

	FNavLocation Location;
	NavigationSystem->ProjectPointToNavigation(InLocation, Location);
	if (Location.Location.IsZero())
	{
		return false;
	}
	return true;
}

FVector WRActorUtility::ProjectPointToNavigation(class AActor* InActor)
{
	if (InActor == nullptr)
		return FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(InActor->GetWorld());
	if (nullptr == NavSystem)
		return FVector::ZeroVector;

	FVector Extent(5.0f, 5.0f, 1000.0f);
	FNavLocation OutNavLocation;
	NavSystem->ProjectPointToNavigation(InActor->GetActorLocation(), OutNavLocation, Extent);

	return OutNavLocation.Location;
}

FVector WRActorUtility::ProjectPointToNavigation(FVector InLocation, FVector InExtent)
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == World)
		return FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(World);
	if (nullptr == NavSystem)
		return FVector::ZeroVector;

	FVector Extent(5.0f, 5.0f, 1000.0f);
	if (FVector::ZeroVector != InExtent)
		Extent = InExtent;
	FNavLocation OutNavLocation;
	NavSystem->ProjectPointToNavigation(InLocation, OutNavLocation, Extent);

	return OutNavLocation.Location;
}

FVector WRActorUtility::ProjectPointToNavigationNotExtent(FVector InLocation)
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == World)
		return FVector::ZeroVector;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(World);
	if (nullptr == NavSystem)
		return FVector::ZeroVector;

	FNavLocation OutNavLocation;
	NavSystem->ProjectPointToNavigation(InLocation, OutNavLocation);

	return OutNavLocation.Location;
}

bool WRActorUtility::ProjectPointToNavigation(AActor* InActor, const FVector& InPoint, FNavLocation& OutNavLocation)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(InActor->GetWorld());
	if (nullptr == NavSystem)
		return false;
	
	FVector Extent(5.0f, 5.0f, 1000.0f);
	bool bResult = NavSystem->ProjectPointToNavigation(InPoint, OutNavLocation, Extent);

	return bResult;
}

#if UE_EDITOR
bool WRActorUtility::IsEditorPreview(class AActor* InActor)
{
	if (InActor == nullptr)
	{
		return false;
	}

	// [ 2019-12-13 : animal14 ] 기본 스포너
	bool IsEditorActor = Cast<AWRCharacter>(InActor) ? false : true;

	// [ 2019-12-13 : animal14 ] 추가 스포너
	IsEditorActor &= Cast<AWRStaticMeshProjectile>(InActor) ? false : true;
	IsEditorActor &= Cast<AWRSlicedActor>(InActor) ? false: true;
	IsEditorActor &= Cast<AWRVehicle>(InActor) ? false : true;

	return IsEditorActor;
}
#endif //UE_EDITOR

void WRActorUtility::SetActorTickEnable(class AActor* InActor, const bool bEnabled)
{
	if (InActor == nullptr)
	{
		return;
	}

	if (bEnabled)
	{
		if (InActor->PrimaryActorTick.IsTickFunctionRegistered() == false)
		{
			InActor->PrimaryActorTick.RegisterTickFunction(InActor->GetWorld()->GetCurrentLevel());
			InActor->PrimaryActorTick.Target = InActor;
		}

		InActor->PrimaryActorTick.bCanEverTick = true;
		InActor->SetActorTickEnabled(true);
	}
	else
	{
		InActor->SetActorTickEnabled(false);
		InActor->PrimaryActorTick.bCanEverTick = false;

		if (InActor->PrimaryActorTick.IsTickFunctionRegistered())
		{
			InActor->PrimaryActorTick.UnRegisterTickFunction();
		}
	}
}

void WRActorUtility::SetComponentTickEnable(class UActorComponent* InComponent, const bool bEnabled)
{
	if (InComponent == nullptr)
	{
		return;
	}

	if (bEnabled)
	{
		if (InComponent->PrimaryComponentTick.IsTickFunctionRegistered() == false)
		{
			InComponent->PrimaryComponentTick.RegisterTickFunction(InComponent->GetWorld()->GetCurrentLevel());
			InComponent->PrimaryComponentTick.Target = InComponent;
		}

		InComponent->PrimaryComponentTick.bCanEverTick = true;
		InComponent->PrimaryComponentTick.SetTickFunctionEnable(true);
	}
	else
	{
		InComponent->PrimaryComponentTick.bCanEverTick = true;
		InComponent->PrimaryComponentTick.SetTickFunctionEnable(false);
		InComponent->PrimaryComponentTick.bCanEverTick = false;

		if (InComponent->PrimaryComponentTick.IsTickFunctionRegistered())
		{
			InComponent->PrimaryComponentTick.UnRegisterTickFunction();
		}
	}
}

void WRActorUtility::SetComponentCollisionEnabled(class UActorComponent* InComponent, ECollisionEnabled::Type NewType)
{
	if (InComponent == nullptr)
	{
		return;
	}

	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(InComponent);
	if (PrimitiveComponent == nullptr)
	{
		return;
	}

	PrimitiveComponent->SetCollisionEnabled(NewType);
}

void WRActorUtility::SetComponentCollisionEnabledToAll(class UActorComponent* InComponent, ECollisionEnabled::Type NewType)
{
	if (InComponent == nullptr)
		return;
	
	auto Propagate = [&]()
	{
		USceneComponent* SceneComponent = Cast<USceneComponent>(InComponent);
		if (SceneComponent == nullptr)
			return;

		TArray<USceneComponent*> ChildrenComponents;
		ChildrenComponents.Append(SceneComponent->GetAttachChildren());

		for (int32 i = 0; i < ChildrenComponents.Num(); ++i)
		{
			WRActorUtility::SetComponentCollisionEnabledToAll(ChildrenComponents[i], NewType);
		}
	};

	SetComponentCollisionEnabled(InComponent, NewType);

	Propagate();
}

void WRActorUtility::SetVisibility(class UActorComponent* InComponent, const bool bInEnable)
{
	if (InComponent == nullptr)
	{
		return;
	}

	USceneComponent* SceneComponent = Cast<USceneComponent>(InComponent);
	if (SceneComponent == nullptr)
	{
		return;
	}

	SceneComponent->SetVisibility(bInEnable);
}

class USkeletalMesh* WRActorUtility::MergeMeshes(const FWRSkeletalMeshMergeParameters& InParameter)
{
	TArray<USkeletalMesh*> MeshesToMerge = InParameter.MeshesToMerge;
	MeshesToMerge.RemoveAll([](USkeletalMesh* InMesh) {
		return InMesh == nullptr;
	});

	if (MeshesToMerge.Num() <= 1)
	{
		return nullptr;
	}

	EMeshBufferAccess MeshBufferAccess = InParameter.bNeedsCpuAccess ? EMeshBufferAccess::ForceCPUAndGPU : EMeshBufferAccess::Default;

	TArray<FSkelMeshMergeSectionMapping> SectionMappings;
	TArray<FSkelMeshMergeUVTransforms> UVTransforms;
	//ToMergeParams(InValue.SectionMappings, SectionMappings);
	//ToMergeParams(InValue.UVTransformsPerMesh, UVTransforms);

	USkeletalMesh* SkeletalMesh = NewObject<USkeletalMesh>();
	if (InParameter.Skeleton && InParameter.bSkeletonBefore)
	{
		SkeletalMesh->Skeleton = InParameter.Skeleton;
	}

	FSkeletalMeshMerge SkeletalMeshMerge(SkeletalMesh, MeshesToMerge, SectionMappings, InParameter.StripTopLODS, MeshBufferAccess, UVTransforms.GetData());
	if (SkeletalMeshMerge.DoMerge() == false)
	{
		return nullptr;
	}

	if (InParameter.Skeleton && InParameter.bSkeletonBefore == false)
	{
		SkeletalMesh->Skeleton = InParameter.Skeleton;
	}

	return SkeletalMesh;
}

void WRActorUtility::SetGenerateOverlapEventsbyActor(const bool bInEnable, AActor* InActor, bool bInPropagateToChild /*= true*/)
{
	if (InActor->IsValidLowLevel() == false)
		return;

	WRActorUtility::SetGenerateOverlapEventbyComponent(bInEnable, InActor->GetRootComponent(), bInPropagateToChild);
}

void WRActorUtility::SetGenerateOverlapEventbyComponent(const bool bInEnable, USceneComponent* InComponent, bool bInPropagateToChild /*= true*/)
{
	if (InComponent == nullptr)
		return;


	auto Propagate = [&]()
	{
		if (bInPropagateToChild == true)
		{
			TArray<USceneComponent*> ChildrenComponents;
			ChildrenComponents.Append(InComponent->GetAttachChildren());

			for (int32 i = 0; i < ChildrenComponents.Num(); ++i)
			{
				WRActorUtility::SetGenerateOverlapEventbyComponent(bInEnable, ChildrenComponents[i], bInPropagateToChild);
			}
		}
	};

	UPrimitiveComponent* SceneToPrimitive = Cast<UPrimitiveComponent>(InComponent);
	if(SceneToPrimitive != nullptr)
	{
		if (SceneToPrimitive->GetGenerateOverlapEvents() != bInEnable)
		{
			SceneToPrimitive->SetGenerateOverlapEvents(bInEnable);
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(SceneToPrimitive);
			if (SkeletalMeshComponent != nullptr)
			{
				SkeletalMeshComponent->bUpdateOverlapsOnAnimationFinalize = bInEnable;
			}
		}
	}

	Propagate();
}

void WRActorUtility::SetEnableUpdateOverlapWhenFinalizeAnim(const bool bInEnable, USceneComponent* InComponent, bool bInPropagateToChild /*= true*/)
{
	if (InComponent == nullptr)
		return;

	auto Propagate = [&]()
	{
		if (bInPropagateToChild == true)
		{
			TArray<USceneComponent*> ChildrenComponents;
			ChildrenComponents.Append(InComponent->GetAttachChildren());

			for (int32 i = 0; i < ChildrenComponents.Num(); ++i)
			{
				WRActorUtility::SetEnableUpdateOverlapWhenFinalizeAnim(bInEnable, ChildrenComponents[i], bInPropagateToChild);
			}
		}
	};

	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InComponent);
	if (SkeletalMeshComponent != nullptr)
	{
		SkeletalMeshComponent->bUpdateOverlapsOnAnimationFinalize = bInEnable;
	}

	Propagate();
}

void WRActorUtility::GetMeshMaterialInstanceDynamic(class UMeshComponent* InMeshComponent, TArray<UMaterialInstanceDynamic*>& OutMIDs)
{
	if (InMeshComponent == nullptr)
		return;

	auto ConvertToMID = [&](UMaterialInterface* InMaterialInterface)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);
		if (MID == nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(InMaterialInterface, InMeshComponent);
		}

		return MID;
	};

	int32 NumofMaterial = InMeshComponent->GetNumMaterials();
	// [ 2019-9-19 : magedoga ] Override Material을 가지고 있다면 그대로 반환
	if (NumofMaterial > 0)
	{
		for (int32 i = 0; i < NumofMaterial; ++i)
		{
			UMaterialInterface* MaterialInterface = InMeshComponent->GetMaterial(i);
			UMaterialInstanceDynamic* MID = ConvertToMID(MaterialInterface);
			if (InMeshComponent->GetMaterial(i) != MID)
				InMeshComponent->SetMaterial(i, MID);

			OutMIDs.Add(MID);
		}
	}

	// [ 2019-9-19 : magedoga ] 없다면 Mesh Material 기준으로 MID 생성 후 Override Material로 세팅
	if (OutMIDs.Num() == 0)
	{
		TArray<UMaterialInterface*> MeshMaterials;
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(InMeshComponent);
		if (StaticMeshComponent != nullptr)
		{
			UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
			if (StaticMesh != nullptr)
			{
				for (int32 i = 0; i < StaticMesh->StaticMaterials.Num(); ++i)
				{
					MeshMaterials.Add(StaticMesh->StaticMaterials[i].MaterialInterface);
				}
			}			
		}

		if (MeshMaterials.Num() == 0)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(InMeshComponent);
			if (SkeletalMeshComponent != nullptr)
			{
				USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->SkeletalMesh;
				if (SkeletalMesh != nullptr)
				{
					for (int32 i = 0; i < SkeletalMesh->Materials.Num(); ++i)
					{
						MeshMaterials.Add(SkeletalMesh->Materials[i].MaterialInterface);
					}
				}
			}
		}

		for (int32 i = 0; i < MeshMaterials.Num(); ++i)
		{
			UMaterialInstanceDynamic* MID = ConvertToMID(MeshMaterials[i]);
			InMeshComponent->SetMaterial(i, MID);
			OutMIDs.Add(MID);
		}
	}
}

float WRActorUtility::GetDegreesBetween2Vectors(const FVector InVector1, const FVector InVector2)
{
	float fDotProduct = FVector::DotProduct(InVector1.GetSafeNormal(), InVector2.GetSafeNormal());
	float fAcos = FMath::Acos(fDotProduct);	// 0~180 degree
	float fConvertDegrees = FMath::RadiansToDegrees(fAcos); //FMath::RadiansToDegrees(acosf(fDotProduct));

	// 좌,우 -180~180 설정
	FVector vCross = FVector::CrossProduct(InVector1.GetSafeNormal(), InVector2.GetSafeNormal());
	if (vCross.Z < 0)
		fConvertDegrees *= -1.0f;

	return fConvertDegrees;
}

bool WRActorUtility::ModifyZHeightForClient(class AActor* InActor, const FVector& Original, float& OutHeight)
{
	OutHeight = Original.Z;

	if (InActor == nullptr)
		return false;

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(InActor->GetRootComponent());
	if (CapsuleComponent == nullptr)
		return false;

	float ScaledHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	OutHeight = Original.Z + ScaledHalfHeight;

	return true;
}

bool WRActorUtility::ModifyZHeightForServer(class AActor* InActor, const FVector& Original, float& OutHeight)
{
	OutHeight = Original.Z;
	if (InActor == nullptr)
		return false;

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(InActor->GetRootComponent());
	if (CapsuleComponent == nullptr)
		return false;

	float ScaledHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	OutHeight = Original.Z - ScaledHalfHeight;

	return true;
}

float WRActorUtility::GetActorCapsuleRadius(class AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return 0.0f;

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(InActor->GetRootComponent());
	if (nullptr == CapsuleComponent)
		return 0.0f;

	return CapsuleComponent->GetScaledCapsuleRadius();
}

float WRActorUtility::GetActorDoNotOverlapCapsuleRadius(class AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return 0.0f;

	TArray<UActorComponent*> CapsuleComponents = InActor->GetComponentsByTag(UCapsuleComponent::StaticClass(), "DoNotOverlap");
	if (CapsuleComponents.Num() == 0)
		return 0.0f;

	UCapsuleComponent* CastedComp = Cast<UCapsuleComponent>(CapsuleComponents[0]);
	if (nullptr == CastedComp)
		return 0.0f;

	return CastedComp->GetScaledCapsuleRadius();
}

float WRActorUtility::GetActorCapsuleHalfHeight(class AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return 0.0f;

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(InActor->GetRootComponent());
	if (nullptr == CapsuleComponent)
		return 0.0f;

	return CapsuleComponent->GetScaledCapsuleHalfHeight();
}

float WRActorUtility::GetLengthBetweenActor(class AWRCharacter* InA, class AWRCharacter* InB, bool bInIncludeRadius, bool bInSize2D)
{
	if (nullptr == InA || nullptr == InB)
		return -1.0f;

	float ARadius = GetActorDoNotOverlapCapsuleRadius(InA);
	float BRadius = GetActorDoNotOverlapCapsuleRadius(InB);

	float fLength = 0.0f;
	if (bInIncludeRadius)
	{
		if (bInSize2D)
		{
			fLength = (InA->GetActorLocation() - InB->GetActorLocation()).Size2D() - ARadius - BRadius;
		}
		else
		{
			fLength = (InA->GetActorLocation() - InB->GetActorLocation()).Size() - ARadius - BRadius;
		}
	}
	else
	{
		if (bInSize2D)
		{
			fLength = (InA->GetActorLocation() - InB->GetActorLocation()).Size2D();
		}
		else
		{
			fLength = (InA->GetActorLocation() - InB->GetActorLocation()).Size();
		}
	}

	return fLength;
}

float WRActorUtility::GetCurrentAttackRange(class AWRCharacter* InActor)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InActor->GetActorID());
	if (nullptr == CombatInfo)
		return 0.0f;

	FWRSkillTable* SkillTable = CombatInfo->GetSkillTable();
	if (nullptr == SkillTable)
		return 0.0f;

	float BodyRadius = GetActorCapsuleRadius(InActor);
	float AttackRange = BodyRadius + SkillTable->Range;

	return AttackRange;
}

bool WRActorUtility::IsAttackableRange(class AWRCharacter* InAttacker, class AWRCharacter* InHitActor)
{
	float HitRadius = GetActorCapsuleRadius(InHitActor) + GetCurrentAttackRange(InAttacker);
	float LengthBetweenActor = GetLengthBetweenActor(InAttacker, InHitActor, true);
	
	if (LengthBetweenActor < HitRadius)
	{
		return true;
	}

	return false;
}

float WRActorUtility::GetDistance(const class AActor* IN InFromActor, const class AActor* IN InToActor)
{
	if (InFromActor == nullptr)
		return 0.f;

	if (InToActor == nullptr)
		return 0.f;

	FVector DistVector = InToActor->GetActorLocation() - InFromActor->GetActorLocation();
	return DistVector.Size();
}

float WRActorUtility::GetDistance(const class AActor* IN InActor, const FVector& IN InVector)
{
	if (InActor == nullptr)
		return 0.f;

	FVector DistVector = InActor->GetActorLocation() - InVector;
	return DistVector.Size();
}

bool WRActorUtility::CheckInnerRange_Sector(const class AWRCharacter* IN InChar, const FTransform IN InStandardTF, const float IN InMin, const float IN InMax, const float IN InFullAngle)
{
	// 영역의 원점과 대상의 원점. Z는 0으로 셋팅 -> Z도 구분을 하고 싶다면 Z체크여부 추가 및 구현하기를..
	FVector vStandardLocation = InStandardTF.GetLocation();
	vStandardLocation.Z = 0;
	FVector vTargetLocation = InChar->GetActorLocation();
	vTargetLocation.Z = 0;

	// 1.거리
	float fDistance = FVector::Dist(vStandardLocation, vTargetLocation);
	if (fDistance < 0)
		fDistance = 0.0f;

	// 2.방향
	FVector vStandardForward = InStandardTF.GetRotation().GetForwardVector();
	FVector vDirToTarget = vTargetLocation - vStandardLocation;
	
	// 3. 영역이 바라보는 방향과 대상으로의 방향으로 각도구함 + 거리로 들어오는지 판단
	// *** Angle은 반각이 아님, 풀각임!
	bool bInner = CheckInnerRange_Sector(vStandardForward, vDirToTarget, fDistance, InMin, InMax, InFullAngle);
	return bInner;
}

bool WRActorUtility::CheckInnerRange_Sector(FVector IN InStandardLocation, FVector IN InDirection, float IN InDistance, float IN InMin, float IN InMax, float IN InFullAngle)
{
	// -Angle이 들어오면 뒤라는 것
	if (InFullAngle < 0.0f)
	{
		InStandardLocation *= -1.0f;
		InFullAngle *= -1.0f;
	}

	if (InDistance >= InMin && InDistance <= InMax)
	{
		float fDot = FVector::DotProduct(InStandardLocation, InDirection.GetSafeNormal());
		float fBetweenAngle = FMath::RadiansToDegrees(FMath::Acos(fDot));
		if (fBetweenAngle <= InFullAngle /2.f)
			return true;
	}
	return false;
}

void WRActorUtility::SetTimer(FTimerHandle& InHandle, float InRate, bool InbLoop)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return;

	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return;

	//기존에 돌고 있던 타이머가 잇으면 클리어 시키고 새로 작성
	if (IsTimerActive(InHandle))
		ClearTimer(InHandle);

	pWorld->GetTimerManager().SetTimer(InHandle, InRate, InbLoop);
}

void WRActorUtility::SetTimer(FTimerHandle& InHandle, FTimerDelegate const& InDelegate, float InRate, bool InbLoop)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return;

	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return;

	pWorld->GetTimerManager().SetTimer(InHandle, InDelegate, InRate, InbLoop);
}

void WRActorUtility::ClearTimer(FTimerHandle& InHandle)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return;

	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return;

	pWorld->GetTimerManager().ClearTimer(InHandle);
}

bool WRActorUtility::IsTimerActive(FTimerHandle InHandle)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return false;

	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return false;

	return pWorld->GetTimerManager().IsTimerActive(InHandle);
}


float WRActorUtility::GetTimerRemaining(FTimerHandle InHandle)
{
	if (UWRGameInstance::GetGameInstance() == nullptr)
		return false;

	UWorld* pWorld = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == pWorld)
		return false;

	return pWorld->GetTimerManager().GetTimerRemaining(InHandle);

}

USplineComponent* WRActorUtility::CreateSplineComponent(class AActor* InActor, bool InbStationaryEndpoints)
{
	USplineComponent* SplineComponent = NewObject<USplineComponent>(InActor);
	if (nullptr == SplineComponent)
		return nullptr;

	SplineComponent->AddToRoot();
	SplineComponent->RegisterComponent();

	SplineComponent->bStationaryEndpoints = InbStationaryEndpoints;

	SplineComponent->bDrawDebug = true;

	return SplineComponent;
}

bool WRActorUtility::DestroySplineComponent(class AActor* InActor, class USplineComponent* InSplineComponent)
{
	if (InSplineComponent == nullptr)
		return false;

	if (InSplineComponent->IsRegistered() == true)
		InSplineComponent->UnregisterComponent();

	InSplineComponent->RemoveFromRoot();

	if (InActor)
	{
		InActor->RemoveOwnedComponent(InSplineComponent);
	}

	InSplineComponent->DestroyComponent();

	return true;
}

int32 WRActorUtility::ConvertTo3PersonMontageTID(const int32 InSrcTID, const int32 InCharTID)
{
	int32 RealMontageTID = InSrcTID;
	RealMontageTID = 5000 + (RealMontageTID % 1000);
	RealMontageTID += DEF_BANDWIDTH_PLAYER_TID * InCharTID;

	return RealMontageTID;
}

int32 WRActorUtility::GetRealMontageTID(AWRCharacter* InTarget, const int32 InBaseTID)
{
	int32 RealMontageTID = InBaseTID;
	if (InTarget->IsPlayer())
	{
		if (InTarget->IsMe())
		{
			AWRCharacterMine* pMe = Cast<AWRCharacterMine>(InTarget);
			if (pMe && pMe->IsShowing3rdCharacter())
			{
				RealMontageTID = ConvertTo3PersonMontageTID(RealMontageTID, InTarget->GetCharacterTableID());
			}
		}
		else
		{
			RealMontageTID = ConvertTo3PersonMontageTID(RealMontageTID, InTarget->GetCharacterTableID());
		}
	}
	return RealMontageTID;
}

void WRActorUtility::DrawDebug(AWRCharacter* InTarget, const EWRDrawDebugType::en& DebugType, FVector InPosition)
{
#if !UE_BUILD_SHIPPING
	if (nullptr == InTarget)
	{
		return;
	}

	if (WRConsoleManager::Get()->GetInt(EWRConsole::Debug_Draw_Position) == 0)
		return;

	FLinearColor DrawColor = FLinearColor::White;
	float DrawTime = 2.0f;
	if (InTarget->IsMe())	//나일때
	{
		DrawColor = FLinearColor::Yellow;
	}
	else if (InTarget->IsPlayer()) // 나 이외의 플레이어일때
	{
		DrawColor = FLinearColor::Green;
	}
	else	// 몹일때
	{
		if (InTarget->GetCharacterType() == EWRCharacterType::NPC
			|| InTarget->GetCharacterType() == EWRCharacterType::ENPC)
		{
			DrawColor = FLinearColor::Red;
		}
		else
		{
			DrawColor = FLinearColor::Gray;
		}
	}

	switch (DebugType)
	{
	case EWRDrawDebugType::Skill:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			FVector Extend = FVector(50.0f, 50.0f, IMCapsuleComponent->GetScaledCapsuleHalfHeight());
			UKismetSystemLibrary::DrawDebugBox(InTarget->GetWorld(), InPosition, Extend, DrawColor, IMCapsuleComponent->GetComponentRotation(), DrawTime, 2.0f);
		}
	}
	break;
	case EWRDrawDebugType::AIMoveTo:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			float ModifiedZ = InPosition.Z;
			ModifyZHeightForClient(InTarget, InPosition, ModifiedZ);

			FVector Start = FVector(InPosition.X, InPosition.Y, ModifiedZ);
			FVector End = Start;
			End.Z = IMCapsuleComponent->GetScaledCapsuleHalfHeight();

			//도착 좌표
			UKismetSystemLibrary::DrawDebugCylinder(InTarget->GetWorld(), Start, End, 100.0f, 12, DrawColor, DrawTime);
		}
	}
	break;
	case EWRDrawDebugType::StopMove:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			//도착 좌표
			UKismetSystemLibrary::DrawDebugCapsule(InTarget->GetWorld(), InPosition, IMCapsuleComponent->GetScaledCapsuleHalfHeight(), IMCapsuleComponent->GetScaledCapsuleRadius(), IMCapsuleComponent->GetComponentRotation(), DrawColor, DrawTime);
		}
	}
	break;
	case EWRDrawDebugType::Reaction:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			//도착 좌표
			UKismetSystemLibrary::DrawDebugCircle(InTarget->GetWorld(), InPosition, IMCapsuleComponent->GetScaledCapsuleHalfHeight(), IMCapsuleComponent->GetScaledCapsuleRadius(), DrawColor, DrawTime, 3.0f);
		}
	}
	break;
	case EWRDrawDebugType::Teleport:
	{

	}
	break;
	case EWRDrawDebugType::HitRange:
	{

	}
	break;
	case EWRDrawDebugType::DodgeRange:
	{

	}
	break;
	case EWRDrawDebugType::Spawn:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			float ModifiedZ = InPosition.Z;
			ModifyZHeightForClient(InTarget, InPosition, ModifiedZ);

			FVector Start = FVector(InPosition.X, InPosition.Y, ModifiedZ);
			FVector End = Start;
			End.Z = IMCapsuleComponent->GetScaledCapsuleHalfHeight();

			//도착 좌표
			UKismetSystemLibrary::DrawDebugCylinder(InTarget->GetWorld(), Start, End, 100.0f, 12, DrawColor, DrawTime);
		}
	}
	break;
	case EWRDrawDebugType::WarpTo:
	{
		UCapsuleComponent* IMCapsuleComponent = Cast<UCapsuleComponent>(InTarget->GetRootComponent());
		if (IMCapsuleComponent)
		{
			//도착 좌표
			UKismetSystemLibrary::DrawDebugCapsule(InTarget->GetWorld(), InPosition, IMCapsuleComponent->GetScaledCapsuleHalfHeight(), IMCapsuleComponent->GetScaledCapsuleRadius(), IMCapsuleComponent->GetComponentRotation(), DrawColor, DrawTime);
		}
	}
	break;
	default:
		break;
	}

#endif // !UE_BUILD_SHIPPING
}

void WRActorUtility::DrawDebug(const EWRDrawDebugType::en& DebugType, FVector InPosition)
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (nullptr == World)
		return;

	FLinearColor DrawColor = FLinearColor::White;
	float DrawTime = 2.0f;
	switch (DebugType)
	{
	case EWRDrawDebugType::Skill:
	{
		FVector Extend = FVector(50.0f, 50.0f, 100.0f);
		UKismetSystemLibrary::DrawDebugBox(World, InPosition, Extend, DrawColor, FRotator::ZeroRotator, DrawTime, 2.0f);
	}
	break;
	case EWRDrawDebugType::AIMoveTo:
	{
		FVector Start = InPosition;
		FVector End = Start;
		End.Z = InPosition.Z + 100.0f;

		//도착 좌표
		UKismetSystemLibrary::DrawDebugCylinder(World, Start, End, 100.0f, 12, DrawColor, DrawTime);
	}
	break;
	case EWRDrawDebugType::StopMove:
	{
		//도착 좌표
		UKismetSystemLibrary::DrawDebugCapsule(World, InPosition, 100.0f, 100.0f, FRotator::ZeroRotator, DrawColor, DrawTime);
	}
	break;
	case EWRDrawDebugType::Reaction:
	{
			//도착 좌표
		UKismetSystemLibrary::DrawDebugCircle(World, InPosition, 100.0f, 100.0f, DrawColor, DrawTime, 3.0f);
	}
	break;
	case EWRDrawDebugType::Teleport:
	{

	}
	break;
	case EWRDrawDebugType::HitRange:
	{

	}
	break;
	case EWRDrawDebugType::DodgeRange:
	{

	}
	break;
	case EWRDrawDebugType::Spawn:
	{
		FVector Start = InPosition;
		FVector End = Start;
		End.Z = InPosition.Z + 200.0f;

		//도착 좌표
		UKismetSystemLibrary::DrawDebugCylinder(World, Start, End, 200.0f, 12, DrawColor, DrawTime);
	}
	break;
	default:
		break;
	}
}

void WRActorUtility::DrawSector(UWorld* pWorld, FVector vStandardLoc, FVector vDir, float fMin, float fMax, float fAngle, float fLifeTime /*= 0.5f*/)
{
	FVector vLeftStart, vLeftEnd;
	vLeftStart = vDir.GetSafeNormal() * fMin;
	vLeftStart = vStandardLoc + vLeftStart.RotateAngleAxis(fAngle / 2.0f * -1.0f, FVector(0, 0, 1));

	vLeftEnd = vDir.GetSafeNormal() * fMax;
	vLeftEnd = vStandardLoc + vLeftEnd.RotateAngleAxis(fAngle / 2.0f * -1.0f, FVector(0, 0, 1));

	FVector vRightStart, vRightEnd;
	vRightStart = vDir.GetSafeNormal() * fMin;
	vRightStart = vStandardLoc + vRightStart.RotateAngleAxis(fAngle / 2.0f, FVector(0, 0, 1));

	vRightEnd = vDir.GetSafeNormal() * fMax;
	vRightEnd = vStandardLoc + vRightEnd.RotateAngleAxis(fAngle / 2.0f, FVector(0, 0, 1));

	UKismetSystemLibrary::DrawDebugLine(pWorld, vLeftStart, vLeftEnd, FColor(255, 0, 0), fLifeTime, 5);
	UKismetSystemLibrary::DrawDebugLine(pWorld, vRightStart, vRightEnd, FColor(0, 255, 0), fLifeTime, 5);

	if (fAngle < 5.0f)
	{
		UKismetSystemLibrary::DrawDebugLine(pWorld, vLeftStart, vRightStart, FColor(255, 0, 0), fLifeTime, 5);
		UKismetSystemLibrary::DrawDebugLine(pWorld, vLeftEnd, vRightEnd, FColor(0, 255, 0), fLifeTime, 5);
	}
	else
	{
		int nPoint = fAngle / 5.0f;

		FVector vPrevPoint = vDir.GetSafeNormal() * fMin;
		vPrevPoint = vPrevPoint.RotateAngleAxis(fAngle / 2.0f * -1.0f, FVector(0, 0, 1));

		for (int i = 0; i < nPoint; ++i)
		{
			FVector vNextPoint = vPrevPoint;
			vNextPoint = vNextPoint.RotateAngleAxis(5.0f, FVector(0, 0, 1));
			UKismetSystemLibrary::DrawDebugLine(pWorld, vStandardLoc + vPrevPoint, vStandardLoc + vNextPoint, FColor(255, 0, 0), fLifeTime, 5);
			vPrevPoint = vNextPoint;
		}

		vPrevPoint = vDir.GetSafeNormal() * fMax;
		vPrevPoint = vPrevPoint.RotateAngleAxis(fAngle / 2.0f * -1.0f, FVector(0, 0, 1));

		for (int i = 0; i < nPoint; ++i)
		{
			FVector vNextPoint = vPrevPoint;
			vNextPoint = vNextPoint.RotateAngleAxis(5.0f, FVector(0, 0, 1));
			UKismetSystemLibrary::DrawDebugLine(pWorld, vStandardLoc + vPrevPoint, vStandardLoc + vNextPoint, FColor(0, 255, 0), fLifeTime, 5);
			vPrevPoint = vNextPoint;
		}
	}
}

class AActor* WRActorUtility::FindActorByAID(AID_t IN InAID)
{
	AWRCharacter* FoundCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
	if (FoundCharacter != nullptr)
		return FoundCharacter;

	AWREnvActor* FoundEnv = WREnvInteractionManager::Get()->FindEnvByAID(InAID);
	if (FoundEnv != nullptr)
		return FoundEnv;

	return nullptr;
}

class AActor* WRActorUtility::FindActorByActorTypeAndAID(EWRActorDetailType IN InActorType, AID_t IN InAID)
{
	if (IsCharacterType(InActorType))
	{
		AWRCharacter* FoundCharacter = WRCharacterManager::Get()->FindbyAID(InAID);
		if (FoundCharacter != nullptr)
		{
			return FoundCharacter;
		}
	}
	else if (IsIOType(InActorType))
	{
		AWREnvActor* FoundEnv = WREnvInteractionManager::Get()->FindEnvByAID(InAID);
		if (FoundEnv != nullptr)
		{
			return FoundEnv;
		}
	}

	return nullptr;
}
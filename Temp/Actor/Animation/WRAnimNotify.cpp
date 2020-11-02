// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAnimNotify.h"

#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimMontage.h>
#include "WRAnimInstance.h"
#include "Animation/SkeletalMeshActor.h"

#include "../../Core/WRGameInstance.h"

#include "../../Manager/WRVFXManager.h"
#include "../../Manager/WRSFXManager.h"
#include "../../Manager/WRCharacterManager.h"
#include "../../Manager/WRInputSystemManager.h"
#include "Manager/WRConsoleManager.h"

#include "Kismet/GameplayStatics.h"

#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"

#include "Component/Effect/WRVFXComponentAttach.h"
#include "Component/Effect/WRVFXComponentMove.h"

#include "Sound/SoundBase.h"

#include "Utility/WRActorUtility.h"

#include "../Character/WRCharacter.h"
#include "../Character/WRCharacterMine.h"
#include "../Character/WRCharacterIO.h"

#include "Table/WRFootstepsTable.h"
#include "Table/WRFootPrintsTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"

#include "UObjectIterator.h"

#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRTODManager.h"

#include "CombatSystem/Combat/WRCombatInfo.h"
#include "../Vehicle/WRVehicle.h"
#include "WRVehicleAnimInstance.h"


void UWRAnimNotify_HitTiming::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	//Super::BranchingPointNotify(BranchingPointPayload);
	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->AnimNotify_HitTiming();
}

void UWRAnimNotify_SkipAnimation::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	//Super::BranchingPointNotify(BranchingPointPayload);

	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->AnimNotify_SkipAnimation();
}

void UWRAnimNotify_SkipAnimation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AWRCharacter* Spawner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (Spawner == nullptr)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(MeshComp->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	AnimInstance->AnimNotify_SkipAnimation();
}

/*
void UWRAnimNotify_DefenseAnimLoopEnd::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	//디펜스가 애니가 끝났는대도 아직 디펜스 중이면
	//몽타주를 처음으로 되돌림

	if (nullptr == BranchingPointPayload.SkelMeshComponent)
		return;

	AWRCharacter* OwnerCharacter = Cast<AWRCharacter>(BranchingPointPayload.SkelMeshComponent->GetOwner());
	if (nullptr == OwnerCharacter)
		return;

	//여전히 방패 모션 중이면
	if (OwnerCharacter->GetCurrentFSMType() != EWRFSM::Defense)
		return;

	UWRAnimInstance* AnimInstance = Cast<UWRAnimInstance>(BranchingPointPayload.SkelMeshComponent->GetAnimInstance());
	if (nullptr == AnimInstance)
		return;

	FAnimMontageInstance* AnimMontageInstance = AnimInstance->GetActiveMontageInstance();
	if (AnimMontageInstance)
	{
		FString CurrentSection = AnimMontageInstance->GetCurrentSection().ToString();
		UAnimMontage* AnimMontage = AnimMontageInstance->Montage;
		float Rate = AnimMontageInstance->GetPlayRate();

		float const Duration = AnimInstance->Montage_Play(AnimMontage, Rate);
		if (0.f < Duration)
		{
			// Start at a given Section.
			if (CurrentSection.IsEmpty() == false)
			{
				//AnimInstance->Montage_JumpToSection(FName(*CurrentSection), AnimMontage);
				AnimInstance->Montage_JumpToSection(FName("beginLoop"), AnimMontage);
			}
		}
	}
}
*/

UWRAnimNotify_VFX::UWRAnimNotify_VFX()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 200, 200, 255);
#endif // WITH_EDITORONLY_DATA
}

FString UWRAnimNotify_VFX::GetNotifyName_Implementation() const
{
	int32 Key = PreviewIndex;
	if (VFXs.Contains(Key) == false)
	{
		return Super::GetNotifyName_Implementation();
	}
	//return VFXs[Key].GeneratedClass.GetAssetName();
	return FPaths::GetBaseFilename(VFXs[Key].Path);
}

void UWRAnimNotify_VFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(MeshComp->GetOwner()))
	{
		this->SpawnParticleSystem(MeshComp, Animation);
	}
	else
#else // UE_EDITOR
#endif // UE_EDITOR
	{
		AWRCharacter* Spawner = Cast<AWRCharacter>(MeshComp->GetOwner());
		if (Spawner == nullptr)
		{
			return;
		}
		AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(Spawner->GetTargetAID());

		UWRActorComponentFSM* ActorComponentFSM = Spawner->GetComponent<UWRActorComponentFSM>();
		if (ActorComponentFSM == nullptr)
		{
			return;
		}

		int32 Key = ActorComponentFSM->GetCurrentFSMInfo()->SkillTableID;
		if (VFXs.Contains(Key) == false)
		{
			return;
		}

		//AWRVFX* VFX = WRVFXManager::Get()->Spawn(VFXs[Key].GeneratedClass.LoadSynchronous(), Spawner, Target, nullptr, EWRVFX::None, VFXs[Key].AdditionalAngle);
		AWRVFX* VFX = WRVFXManager::Get()->Spawn(VFXs[Key].Path, Spawner, Target, nullptr, EWRVFX::None, VFXs[Key].AdditionalAngle);
		if (VFX == nullptr)
		{
			return;
		}

		// [ 2019-8-22 : animal14 ] 노티파이에서 루핑되는 VFX 및 SFX를 사용X
		if (VFX->IsLooping() == false)
		{
			return;
		}

#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(Animation));
		StringArgs.Add(GetNameSafe(VFX));

		FString Text = FString::Format(TEXT("Anim {0} tried to spawn infinitely looping effect asset {1}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_VFX::Notify()"));
#else // UE_EDITOR		
#endif // UE_EDITOR

		WRVFXManager::Get()->Remove(VFX);
	}
}

//void UWRAnimNotify_VFX::PreSave(const class ITargetPlatform* TargetPlatform)
//{
//	for (auto& VFX : VFXs)
//	{
//		if (VFX.Value.GeneratedClass.IsNull())
//		{
//			continue;
//		}
//
//		VFX.Value.Path = VFX.Value.GeneratedClass.ToString();
//		VFX.Value.GeneratedClass.Reset();
//	}
//
//	Super::PreSave(TargetPlatform);
//}

#if UE_EDITOR
void UWRAnimNotify_VFX::SpawnParticleSystem(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	int32 Key = PreviewIndex;
	if (VFXs.Contains(Key) == false)
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	AActor* Spawner = MeshComp->GetOwner();
	if (Spawner == nullptr)
	{
		return;
	}

	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	//AWRVFX* VFX = World->SpawnActor<AWRVFX>(VFXs[Key].GeneratedClass.LoadSynchronous(), FVector::ZeroVector, FRotator::ZeroRotator, Parameters);
	AWRVFX* VFX = World->SpawnActor<AWRVFX>(WRActorSpawnUtility::GetGeneratedClass<AWRVFX>(*VFXs[Key].Path), FVector::ZeroVector, FRotator::ZeroRotator, Parameters);
	if (VFX == nullptr)
	{
		return;
	}

	this->OnComplete(VFX, Spawner, nullptr, nullptr, VFXs[Key].AdditionalAngle);

	// [ 2019-8-22 : animal14 ] 노티파이에서 루핑되는 VFX 및 SFX를 사용X
	if (VFX->IsLooping() == false)
	{
		return;
	}

	TArray<FStringFormatArg> StringArgs;
	StringArgs.Add(GetNameSafe(Animation));
	StringArgs.Add(GetNameSafe(VFX));

	FString Text = FString::Format(TEXT("Anim {0} tried to spawn infinitely looping effect asset {1}."), StringArgs);

	FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_VFX::Notify()"));

	VFX->SetActive(false);
	VFX->Destroy();
	VFX = nullptr;
}

void UWRAnimNotify_VFX::OnComplete(AWRVFX* InVFX, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const FRotator& InAdditionalAngle)
{
	InVFX->Initialize(InSpawner, InTarget, InParent, EWRVFX::None, InAdditionalAngle);
	InVFX->SetLifespan4Preview();
}
#else // UE_EDITOR
#endif // UE_EDITOR

UWRAnimNotify_SFX::UWRAnimNotify_SFX() : Super()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(200, 200, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

FString UWRAnimNotify_SFX::GetNotifyName_Implementation() const
{
	int32 Key = PreviewIndex;
	//if (SFXs.Contains(Key) == false)
	//{
	//	return Super::GetNotifyName_Implementation();
	//}
	//return SFXs[Key].GetAssetName();
	if (Paths.Contains(Key) == false)
	{
		return Super::GetNotifyName_Implementation();
	}
	return FPaths::GetBaseFilename(Paths[Key]);
}

void UWRAnimNotify_SFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(MeshComp->GetOwner()))
	{
		this->SpawnAudio(MeshComp, Animation);
	}
	else
#else // UE_EDITOR
#endif // UE_EDITOR
	{
		AWRCharacter* Spawner = Cast<AWRCharacter>(MeshComp->GetOwner());
		if (Spawner == nullptr)
		{
			return;
		}

		if (Spawner->GetIsCharacterActive() == false)
			return;

		UWRActorComponentFSM* ActorComponentFSM = Spawner->GetComponent<UWRActorComponentFSM>();
		if (ActorComponentFSM == nullptr)
		{
			return;
		}

		int32 Key = ActorComponentFSM->GetCurrentFSMInfo()->SkillTableID;
		//if (SFXs.Contains(Key) == false)
		if (Paths.Contains(Key) == false)
		{
			return;
		}

		//USoundBase* SoundBase = Cast<USoundBase>(SFXs[Key].LoadSynchronous());
		USoundBase* SoundBase = FindObject<USoundBase>(nullptr, *Paths[Key]);
		if (SoundBase == nullptr)
		{
			SoundBase = LoadObject<USoundBase>(nullptr, *Paths[Key]);
			return;
		}

		// [ 2019-8-22 : animal14 ] 노티파이에서 루핑되는 VFX 및 SFX를 사용X
		if (SoundBase->IsLooping())
		{
#if UE_EDITOR
			TArray<FStringFormatArg> StringArgs;
			StringArgs.Add(GetNameSafe(Animation));
			StringArgs.Add(GetNameSafe(SoundBase));

			FString Text = FString::Format(TEXT("Anim {0} tried to spawn infinitely looping sound asset {1}."), StringArgs);

			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_SFX::Notify()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
			return;
		}

		WRSFXManager::Get()->SpawnAtLocation(SoundBase, Spawner->GetActorLocation());
	}
}

//void UWRAnimNotify_SFX::PreSave(const class ITargetPlatform* TargetPlatform)
//{
//	for (auto& SFX : SFXs)
//	{
//		if (SFX.Value.IsNull())
//		{
//			continue;
//		}
//
//		Paths.Add(SFX.Key, SFX.Value.ToString());
//		SFX.Value.Reset();
//	}
//
//	Super::PreSave(TargetPlatform);
//}

void UWRAnimNotify_SFX::SpawnAudio(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	if (MeshComp == nullptr)
		return;

	int32 Key = PreviewIndex;
	if (Paths.Contains(Key) == false)
		//if (SFXs.Contains(Key) == false)
	{
		return;
	}

	//USoundBase* SoundBase = Cast<USoundBase>(SFXs[Key].LoadSynchronous());
	USoundBase* SoundBase = LoadObject<USoundBase>(nullptr, *Paths[Key]);
	if (SoundBase == nullptr)
	{
		return;
	}

	// [ 2019-8-22 : animal14 ] 노티파이에서 루핑되는 VFX 및 SFX를 사용X
	if (SoundBase->IsLooping())
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(GetNameSafe(Animation));
		StringArgs.Add(GetNameSafe(SoundBase));

		FString Text = FString::Format(TEXT("Anim {0} tried to spawn infinitely looping sound asset {1}."), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("UWRAnimNotify_SFX::Notify()"));
#else // UE_EDITOR		
#endif // UE_EDITOR
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	AActor* Spawner = MeshComp->GetOwner();
	if (Spawner == nullptr)
	{
		return;
	}

	UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(World, SoundBase, Spawner->GetActorLocation());
}

UWRAnimNotify_Interact::UWRAnimNotify_Interact()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(200, 255, 200, 255);
#endif // WITH_EDITORONLY_DATA
}

void UWRAnimNotify_Interact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AWRCharacterIO* Spawner = Cast<AWRCharacterIO>(MeshComp->GetOwner());
	if (Spawner == nullptr)
	{
		return;
	}

	switch (Event)
	{
	case EWRInteractEvent::Throw:
		this->Throw(Spawner);
		break;
	}
}

void UWRAnimNotify_Interact::Throw(class AWRCharacterIO* InSpawner)
{
	// [ 2020-1-10 : animal14 ] 객체 생성
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}

	float Lifespan = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_Lifespan);

	ASkeletalMeshActor* SkeletalMeshActor = World->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass());
	if (SkeletalMeshActor == nullptr)
	{
		return;
	}

	AWRCharacter* Character = WRCharacterManager::Get()->GetMyCharacter();
	if (Character == nullptr)
	{
		return;
	}

	FVector Direction = Character->GetActorLocation() - InSpawner->GetMeshParentComponent()->GetComponentLocation();
	Direction.Normalize();

	FVector Location = InSpawner->GetMeshParentComponent()->GetComponentLocation() + (Direction * 200.0F);

	SkeletalMeshActor->SetActorLocation(Location);
	SkeletalMeshActor->SetLifeSpan(Lifespan);

	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(World, TEXT("/Game/Asset/Character/IO/bridge/Bridge_IO02.Bridge_IO02"));
	if (SkeletalMesh == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = SkeletalMeshActor->GetSkeletalMeshComponent();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}
	SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);

	// [ 2020-1-10 : animal14 ] 방향 계산
	float Degree = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_ThrowDegree);

	FRotator Rotation = FRotator(0.0F, Degree, 0.0F);
	Direction = Rotation.RotateVector(Direction);

	// [ 2020-1-10 : animal14 ] 물리 적용
	float Speed = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_ThrowSpeed);

	SkeletalMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->AddImpulse(Direction * Speed, NAME_None, true);
}

UWRAnimNotify_Footsteps::UWRAnimNotify_Footsteps()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(200, 200, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

/*
void UWRAnimNotify_Footsteps::ParseDataTable()
{
	if (FootstepsTable != nullptr)
	{
		FootstepsMap.Empty();

		TArray<FWRFootstepsTable*> tempArray;
		FootstepsTable->GetAllRows(FString(""), tempArray);

		for (FWRFootstepsTable* InData : tempArray)
		{
			EPhysicalSurface InSurfaceEnum = InData->SurfaceType;

			FootstepsMap.Add(InSurfaceEnum, InData);
		}
	}
}
*/

void UWRAnimNotify_Footsteps::SpawnFootstepSFX(const FHitResult& InResult, const EPhysicalSurface& InIndex)
{
	FWRFootstepsTable* InFootstepTable = WRTableManager::Get()->FindRow<FWRFootstepsTable>(InIndex);
	if (InFootstepTable == nullptr)
		return;

	EPhysicalSurface InSurfaceEnum = InFootstepTable->SurfaceType;

	if (InFootstepTable->SurfaceType != InIndex)
		return;

	FString InStepSoundPath;

	if (WRTODManager::Get()->IsRaining() == false)
	{
		InStepSoundPath = InFootstepTable->StepSound;
	}
	else
	{
		InStepSoundPath = InFootstepTable->StepSoundInRaining;
	}

	if (InStepSoundPath.IsEmpty() == true)
		return;

	USoundBase* SoundBase = FindObject<USoundBase>(nullptr, *InStepSoundPath);
	if (SoundBase == nullptr)
	{
		SoundBase = LoadObject<USoundBase>(nullptr, *InStepSoundPath);
	}

	if (SoundBase == nullptr)
	{
		return;
	}
	WRSFXManager::Get()->SpawnAtLocation(SoundBase, InResult.Location);
}

void UWRAnimNotify_Footsteps::SpawnFootstepVFX(const FHitResult& InResult, const EPhysicalSurface& InIndex)
{
}

void UWRAnimNotify_Footsteps::SpawnFootPrint(USkeletalMeshComponent* MeshComp, const FHitResult & InResult, const EPhysicalSurface & InIndex)
{
	if (InResult.GetActor() == nullptr)
		return;

	AWRCharacter* InCharacter = Cast<AWRCharacter>(MeshComp->GetOwner());

	if (InCharacter == nullptr)
		return;

	AID_t InCharacterTID = InCharacter->GetCharacterTableID();

	FWRFootPrintsTable* InFootprintTable = WRTableManager::Get()->FindRow<FWRFootPrintsTable>(InIndex);
	if (InFootprintTable == nullptr)
		return;

	if (InFootprintTable->CharacterTID == InCharacterTID)
	{
		if (InFootprintTable->DecalFootPrintMat.IsEmpty() == true)//if (InData->DecalFootPrintMat.IsNull() == true)
			return;

		UWorld* InWorld = InCharacter->GetWorld();

		if (InWorld == nullptr)
			return;

		UMaterialInstance* InFootPrintMat = LoadObject<UMaterialInstance>(nullptr, *InFootprintTable->DecalFootPrintMat); //InData->DecalFootPrintMat.LoadSynchronous();

		if (InFootPrintMat == nullptr)
			return;

		USkeletalMesh* InSkeleton = MeshComp->SkeletalMesh;

		if (InSkeleton == nullptr)
			return;

		int32 CurrentBoneIndex = MeshComp->GetBoneIndex(CurrentBoneName);

		FVector FootLocation = InResult.ImpactPoint;

		bool IsRight = false;

		if (CurrentBoneIndex != INDEX_NONE)
		{
			if (InSkeleton->RefSkeleton.IsValidIndex(CurrentBoneIndex) == true)
			{
				FTransform InBoneTransform = InSkeleton->RefSkeleton.GetRefBonePose()[CurrentBoneIndex];

				FootLocation.X = InBoneTransform.GetLocation().X;
				FootLocation.Y = InBoneTransform.GetLocation().Y;

				if (InBoneTransform.GetLocation().X > 0.f)
				{
					IsRight = true;
				}
			}
		}

		FRotator InRot = InResult.ImpactNormal.Rotation();

		InRot.Yaw = InCharacter->GetActorRotation().Yaw + 90.f;

		if (CurrentBoneName.ToString().Contains(TEXT("_r")) || IsRight == true)
			InRot.Pitch += 180.f;

		UGameplayStatics::SpawnDecalAtLocation(InWorld, InFootPrintMat, FVector::OneVector * InFootprintTable->FootPrintSize, FootLocation, InRot, WRConsoleManager::Get()->GetFloat(EWRConsole::FootPrintDuration));
		
	}

}

void UWRAnimNotify_Footsteps::SpawnFootstepSFXInEditor(USkeletalMeshComponent* MeshComp)
{
	UDataTable* FootstepsTable = nullptr;

	FSoftObjectPath SoftObjectPath(TEXT("/Game/Asset/Data/Table/CharData_Footsteps.CharData_Footsteps"));

	if (SoftObjectPath.IsValid() == false)
		return;

	UObject* InDataObject = SoftObjectPath.TryLoad();

	if (InDataObject == nullptr)
		return;

	FootstepsTable = Cast<UDataTable>(InDataObject);

	if (FootstepsTable == nullptr)
		return;

	TArray<FWRFootstepsTable*> TempArray;
	FootstepsTable->GetAllRows(FString(""), TempArray);

	for (FWRFootstepsTable* InData : TempArray)
	{
		EPhysicalSurface InSurfaceEnum = InData->SurfaceType;

		if (InData->SurfaceType == PreviewIndex)
		{
			FString SoundPath;// FSoftObjectPath SoundPath;

			if (WRTODManager::Get()->IsRaining() == false)
			{
				SoundPath = InData->StepSound;// .ToSoftObjectPath();
			}
			else
			{
				SoundPath = InData->StepSoundInRaining;// .ToSoftObjectPath();
			}


			//if (SoundPath.IsValid())
			//{
				//UObject* InSoundObject = SoundPath.TryLoad();

				//if (InSoundObject == nullptr)
					//return;

			UWorld* InWorld = MeshComp->GetWorld();

			if (InWorld == nullptr)
				return;

			USoundBase* InSoundBase = LoadObject<USoundBase>(nullptr, *SoundPath);//Cast<USoundBase>(InSoundObject);
			if (InSoundBase == nullptr)
				return;


			AActor* Spawner = MeshComp->GetOwner();
			if (Spawner == nullptr)
			{
				return;
			}

			UGameplayStatics::SpawnSound2D(InWorld, InSoundBase);
			//UGameplayStatics::SpawnSoundAtLocation(InWorld, InSoundBase, Spawner->GetTargetLocation());
		//}
		}
	}
}

FString UWRAnimNotify_Footsteps::GetNotifyName_Implementation() const
{
	if (PrePreviewIndex != this->PreviewIndex)
	{
		for (TObjectIterator<UWRAnimNotify_Footsteps> it; it; ++it)
		{
			UWRAnimNotify_Footsteps* TargetFootstep = *it;
			TargetFootstep->PreviewIndex = this->PreviewIndex;
			TargetFootstep->PrePreviewIndex = this->PreviewIndex;
		}
	}

	return Super::GetNotifyName_Implementation();
}

void UWRAnimNotify_Footsteps::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* InCharacter = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (InCharacter == nullptr)
		return;

	//[2020-08-31 EZ] 비밀대화 투명화 처리 되고나서 풋스템 사운트 Off
	if (InCharacter->GetIsCharacterActive() == false)
		return;

	const FVector CharLocation = MeshComp->GetOwner()->GetActorLocation();

	const FVector DownDirection = CharLocation - FVector(0, 0, 500.0f);

	FHitResult InHitResult;

	ECollisionChannel InColChannel = ECollisionChannel::ECC_Visibility;

#if UE_EDITOR
	if (MeshComp->GetWorld()->IsPreviewWorld() == true)
	{
		this->SpawnFootstepSFXInEditor(MeshComp);
		return;
	}
#endif //UE_EDITOR

	UWorld* InWorld = UWRGameInstance::GetGameInstance()->GetWorld();

	if (InWorld == nullptr)
		return;

	FCollisionQueryParams TraceParams(FName(TEXT("Footsteps trace")), true, MeshComp->GetOwner());
	TraceParams.bReturnPhysicalMaterial = true;

	bool bHitResult = InWorld->LineTraceSingleByChannel(InHitResult,
		CharLocation,
		DownDirection,
		InColChannel,
		TraceParams);

	if (bHitResult == true)
	{
		EPhysicalSurface InSurfaceType = UGameplayStatics::GetSurfaceType(InHitResult);

		SpawnFootstepSFX(InHitResult, InSurfaceType);
		//SpawnfootstepVFX(InHitResult);
		SpawnFootPrint(MeshComp, InHitResult, InSurfaceType);
	}
}

void UWRAnimNotify_ForceFeedback::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

#if UE_EDITOR
	if (WRActorUtility::IsEditorPreview(MeshComp->GetOwner()))
	{
		return;
	}
#endif //UE_EDITOR

	if (WRInputSystemManager::IsValid() == true && WRCharacterManager::IsValid() == true)
	{
		AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
		if (BroadCast == false)
		{
			if (pOwner != nullptr)
			{
				// [ 2019-12-5 : hansang88 ] 발동 주체가 나일경우 진동
				if (pOwner->GetActorID() == WRCharacterManager::Get()->GetMyCharacter()->GetActorID())
				{
					WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedbackTID);
				}
			}
		}
		else
		{
			// [ 2019-12-5 : hansang88 ] 발동 주체와 거리 체크
			if (BroadCastDistance != 0)
			{
				AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
				if (pOwner != nullptr && MyChar != nullptr)
				{
					float Distance = MyChar->GetDistanceTo(pOwner);
					if (Distance < BroadCastDistance)
					{
						WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedbackTID);
					}
				}
			}
			else
			{
				WRInputSystemManager::Get()->ClientPlayForceFeedback(ForceFeedbackTID);
			}
		}
	}

}

void UWRAnimNotify_HookThrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (pOwner == nullptr)
		return;


	switch (ThrowEvent)
	{
	case EWRThrowEventType::Hook:
	{
		pOwner->OnHookThrow();
	}
	break;
	case EWRThrowEventType::Shield:
	{
		WRTableID SubWeaponTID = pOwner->GetLeftWeaponTID();
		FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(SubWeaponTID);
		if (nullptr == WeaponTable)
			return;

		FWREquipmentLookTable* LookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(WeaponTable->WeaponLookTid);
		if (nullptr == LookTable)
			return;

		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(pOwner->GetActorID());
		if (CombatInfo)
		{
			CombatInfo->OnNotifyThrow(WeaponTable->WeaponLookTid);
		}
	}
	break;
	default:
		break;
	}
}

void UWRAnimNotify_HookEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (pOwner == nullptr)
		return;

	pOwner->OnHookEventToCaster();
}

void UWRAnimNotify_ChangeWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (pOwner == nullptr)
		return;

	pOwner->ChangeWeaponWithoutAnimation(EquipPart);
}

void UWRAnimNotify_AnimEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (pOwner == nullptr)
		return;

	pOwner->OnAnimEnded(Animation);
}

void UWRAnimNotify_Exposure::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRCharacter* pOwner = Cast<AWRCharacter>(MeshComp->GetOwner());
	if (pOwner == nullptr)
		return;

	if (WRMaterialParameterCollectionManager::IsValid() == false)
		return;

	WRMaterialParameterCollectionManager::Get()->SetExposureTime(ExposureTime);
}

UWRAnimNotify_TempDrunk::UWRAnimNotify_TempDrunk()
{
}

FString UWRAnimNotify_TempDrunk::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UWRAnimNotify_TempDrunk::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (WRCharacterManager::IsValid() == false)
	{
		return;
	}

	AWRCharacterMine* Me = WRCharacterManager::Get()->GetMyCharacter();
	if (Me == nullptr)
	{
		return;
	}

	Me->StartDrunk(Duration);
}

void UWRAnimNotify_SteeringStartAutoPilot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRVehicle* pOwner = Cast<AWRVehicle>(MeshComp->GetOwner());
	if (pOwner)
	{
		TArray<UActorComponent*> SteeringWheels = pOwner->GetComponentsByTag(UActorComponent::StaticClass(), "SteeringWheel");
		for (int32 Idx = 0; Idx < SteeringWheels.Num(); ++Idx)
		{
			USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(SteeringWheels[Idx]);
			if (nullptr == MeshComponent)
				continue;

			UWRSteeringWheelAnimInstance* SteeringAnimInstance = Cast<UWRSteeringWheelAnimInstance>(MeshComponent->GetAnimInstance());
			if (nullptr == SteeringAnimInstance)
				continue;

			SteeringAnimInstance->SWheelState = EWRSteeringWheelState::AutoIdle;
		}
	}
}

void UWRAnimNotify_SteeringEndAutoPilot::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner() == nullptr)
		return;

	AWRVehicle* pOwner = Cast<AWRVehicle>(MeshComp->GetOwner());
	if (pOwner)
	{
		TArray<UActorComponent*> SteeringWheels = pOwner->GetComponentsByTag(UActorComponent::StaticClass(), "SteeringWheel");
		for (int32 Idx = 0; Idx < SteeringWheels.Num(); ++Idx)
		{
			USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(SteeringWheels[Idx]);
			if (nullptr == MeshComponent)
				continue;

			UWRSteeringWheelAnimInstance* SteeringAnimInstance = Cast<UWRSteeringWheelAnimInstance>(MeshComponent->GetAnimInstance());
			if (nullptr == SteeringAnimInstance)
				continue;

			SteeringAnimInstance->SWheelState = EWRSteeringWheelState::ManualIdle;
		}
	}
}

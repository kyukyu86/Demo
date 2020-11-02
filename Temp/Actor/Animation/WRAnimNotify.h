// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"
#include "Enum/EWRCharacter.h"
#include "WRAnimNotify.generated.h"

//====================================================================================
USTRUCT(BlueprintType)
struct FVFXAnimNotify
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSoftClassPtr<class AWRVFX> GeneratedClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Generated Class")
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator AdditionalAngle;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_HitTiming : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_SkipAnimation : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

//====================================================================================
// UCLASS()
// class WR_API UWRAnimNotify_DefenseAnimLoopEnd: public UAnimNotify
// {
// 	GENERATED_BODY()
// 
// public:
// 	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
// };

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_VFX : public UAnimNotify
{
	GENERATED_BODY()

public:
	UWRAnimNotify_VFX();

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	//virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;

#if UE_EDITOR
private:
	void SpawnParticleSystem(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	void OnComplete(AWRVFX* InVFX, AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const FRotator& InAdditionalAngle);
#else // UE_EDITOR
#endif // UE_EDITOR

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_VFX")
	int32 TargetIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_VFX")
	int32 PreviewIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_VFX")
	TMap<int32, FVFXAnimNotify> VFXs;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_SFX : public UAnimNotify
{
	GENERATED_BODY()

public:
	UWRAnimNotify_SFX();

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	//virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;

private:
	void SpawnAudio(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_SFX")
	int32 PreviewIndex = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_SFX")
	//TMap<int32, TSoftObjectPtr<class USoundCue>> SFXs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_SFX", DisplayName = "SFXs")
	TMap<int32, FString> Paths;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_TempDrunk : public UAnimNotify
{
	GENERATED_BODY()

public:
	UWRAnimNotify_TempDrunk();

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_SFX")
	float Duration = 1.0F;

};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_Interact : public UAnimNotify
{
	GENERATED_BODY()

public:
	UWRAnimNotify_Interact();

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	void Throw(class AWRCharacterIO* InSpawner);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_Interact")
	EWRInteractEvent Event = EWRInteractEvent::Throw;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_Footsteps : public UAnimNotify
{
	GENERATED_BODY()

public:
	UWRAnimNotify_Footsteps();

private:

	void SpawnFootstepSFX(const FHitResult& InResult, const EPhysicalSurface& InIndex);
	void SpawnFootstepVFX(const FHitResult& InResult, const EPhysicalSurface& InIndex);

	void SpawnFootPrint(USkeletalMeshComponent* MeshComp, const FHitResult& InResult, const EPhysicalSurface& InIndex);
	void SpawnFootstepSFXInEditor(USkeletalMeshComponent* MeshComp);

public:
	TEnumAsByte<EPhysicalSurface> PrePreviewIndex;
	
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_Footstep")
	TEnumAsByte<EPhysicalSurface> PreviewIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAnimNotify_Footstep")
	FName CurrentBoneName = FName("");
};

//====================================================================================

UCLASS()
class WR_API UWRAnimNotify_ForceFeedback : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_ForceFeedback")
	int32 ForceFeedbackTID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_ForceFeedback")
	bool BroadCast = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_ForceFeedback")
	float BroadCastDistance = 0.f;
};

//====================================================================================
UENUM(BlueprintType)
enum class EWRThrowEventType : uint8
{
	Hook,
	Shield,
};

UCLASS()
class WR_API UWRAnimNotify_HookThrow : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_HookThrow")
		EWRThrowEventType ThrowEvent = EWRThrowEventType::Hook;
};

UCLASS()
class WR_API UWRAnimNotify_HookEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_ChangeWeapon : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_ChangeWeapon")
		EWRChangeEquipment EquipPart = EWRChangeEquipment::SubWeapon;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_AnimEnded : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_Exposure : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRAnimNotify_Exposure")
		float ExposureTime = 1.0f;
};

//====================================================================================
UCLASS()
class WR_API UWRAnimNotify_SteeringStartAutoPilot: public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
};

UCLASS()
class WR_API UWRAnimNotify_SteeringEndAutoPilot : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
};
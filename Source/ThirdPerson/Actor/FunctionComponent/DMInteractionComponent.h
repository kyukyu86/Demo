// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SphereComponent.h>
#include "../../Enum/DMActorEnum.h"
#include "DMInteractionComponent.generated.h"


#define DEF_INTERACTION_INPUT_INVALID_INDEX -1

UENUM(Meta = (BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDMInteractionFunctionFlag : uint8
{
	None = 0x0000,

	Widget = 0x0001,
	Targeting = 0x0002,
	Input = 0x0004,
};
ENUM_CLASS_FLAGS(EDMInteractionFunctionFlag)


USTRUCT(BlueprintType)
struct FDMInteractionResult
{
	GENERATED_BODY()

private:
	bool bDone = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDMInteractionResult ResultType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ResultValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ResultDelayTime = 0;

	int32 ResultIndex = 0;
	FTimerHandle TimerHandle;

	void SetDone(const bool IN InSet) { bDone = InSet; }
	bool IsDone() {	return bDone; }
};

USTRUCT(BlueprintType)
struct FDMInteractionResultForm
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 InteractionableCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<class UAnimSequenceBase> ResultAnimAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDMInteractionResult> ResultList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDMInteractionResultAfter ResultAfter = EDMInteractionResultAfter::Remain;

	int32 InteractionCount = 0;

	bool IsDone() { return InteractionCount >= InteractionableCount; }
	void AddInteracitonCount() { ++InteractionCount; }
};

USTRUCT(BlueprintType)
struct FDMInteractionInput
{
	GENERATED_BODY()

public:
	bool bDoing = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDMInput InputKey = EDMInput::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EInputEvent> InputEvent = EInputEvent::IE_Pressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDMInteractionResultForm ResultForm;

	int32 InputEventIndex = 0;

	void SetDoing(const bool IN InSet) { bDoing = InSet; }
	bool IsDoing() { return bDoing; }
};


UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class THIRDPERSON_API UDMInteractionComponent : public USphereComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Function", meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "EDMInteractionFunctionFlag"))
		int32 FunctionFlags = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Interaction", meta = (AllowPrivateAccess = "true"))
		float InteractionDistance = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Interaction", meta = (AllowPrivateAccess = "true"))
		bool bDebugInteractionArea = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Widget", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Widget", meta = (AllowPrivateAccess = "true"))
		FVector WidgetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Widget", meta = (AllowPrivateAccess = "true"))
		FRotator WidgetRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Widget", meta = (AllowPrivateAccess = "true"))
		FVector WidgetScale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Input", meta = (AllowPrivateAccess = "true"))
		TArray<FDMInteractionInput> InputEventList;

private:		
	AActor* ParentActor = nullptr;
	bool bIsInteractionableDistance = false;
	int32 CurrentInputEventIndex = DEF_INTERACTION_INPUT_INVALID_INDEX;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	bool OnInputEventProcess(const EDMInput IN InInputType, const EInputEvent IN InEventType);

	// Result
	void OnResultProcess(FDMInteractionResultForm& IN InResultForm);
	void OnResultProcess_Implementation(const int32 InResultIndex, const bool InIsTimerFinished = false);
	void DetermineResultAfter();
	bool SetCurrentInputResultDone(const int32 IN InResultIndex, const bool IN InSet);
	bool GetResultData(const int32 IN InResultIndex, FDMInteractionResult& OUT OutResultData);

// 	UFUNCTION()
// 		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// 	UFUNCTION()
// 		void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	FORCEINLINE AActor* GetParentActor() { return ParentActor; }
	bool IsContainedFlag(const EDMInteractionFunctionFlag IN InFlag);
	bool IsContainedInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);

	void Initialize(AActor* IN InParentActor);
	void Release();

	// Input
	bool OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);

	// Target
	void OnTargeted();
	void OnTargetReleased();
};

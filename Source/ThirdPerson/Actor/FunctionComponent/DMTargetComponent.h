// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DMTargetComponent.generated.h"

UENUM(Meta = (BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDMTargetFunctionFlag : uint8
{
	None = 0x0000,

	Widget = 0x0001,
	Input = 0x0002,
};
ENUM_CLASS_FLAGS(EDMTargetFunctionFlag)


UCLASS(Blueprintable, editinlinenew, meta = (BlueprintSpawnableComponent))
class THIRDPERSON_API UDMTargetComponent : public USphereComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup|Function", meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "EDMTargetFunctionFlag"))
		int32 FunctionFlags = 0;

	AActor* ParentActor = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	void Initialize(AActor* IN InParentActor);
	AActor* GetParentActor() { return ParentActor; }

	void OnTargeted();
	void OnTargetReleased();
};

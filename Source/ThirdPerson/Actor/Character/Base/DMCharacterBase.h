// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../Interface/DMComponentInterface.h"
#include "../../../Enum/DMActorEnum.h"
#include "../../../Struct/DMActorStruct.h"
#include "DMCharacterBase.generated.h"


USTRUCT(BlueprintType)
struct FDMMontageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* Montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SectionName;
};


class UAnimMontage;


UCLASS()
class THIRDPERSON_API ADMCharacterBase : public ACharacter, public IDMComponentInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup|Camera", meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup|Camera", meta = (AllowPrivateAccess = "true"))
		float BaseLookUpRate;

protected:
	FString strAsyncKeyForUI = "";

public:
	// Sets default values for this character's properties
	ADMCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BuildCustomComponents() override;

	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);
	virtual void TurnAtRate(float Rate);
	virtual void LookUpAtRate(float Rate);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float MontagePlay(UAnimMontage* IN InMontage, const FString SectionName = "", float IN InPlayRate = 1.f);
	float MontagePlay(FName IN InRowName);
	
	UFUNCTION(BlueprintCallable)
		void OnMontageEnded(UAnimMontage* InMontage, bool InbInterrupted);

	bool DetermineFSM(const FDMFSMData IN InFSMData);

};

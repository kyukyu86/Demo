// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "../../Animation/AnimInstance/DMPlayerAnimInstance.h"
#include "../../../Component/DMComponentFSM.h"
#include "../../../Component/DMComponentInteraction.h"
#include "../../../Manager/DMFSMManager.h"
#include "../../../Manager/DMCharacterManager.h"
#include "../../../Data/CustomData/DMMontageTable.h"

// Sets default values
ADMCharacterBase::ADMCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void ADMCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	BuildCustomComponents();

	UDMPlayerAnimInstance* AnimInstance = Cast<UDMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ADMCharacterBase::OnMontageEnded);
	}

	DMFSMManager::Get()->DetermineFSM(this, EDMFSMType::Idle);
}

void ADMCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->RemoveComponentAll();

	Super::EndPlay(EndPlayReason);
}

void ADMCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADMCharacterBase::BuildCustomComponents()
{	
	CreateComponent<UDMComponentFSM>(EDMComponentType::FSM, true);
	CreateComponent<UDMComponentInteraction>(EDMComponentType::Interaction, true);
}

void ADMCharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADMCharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADMCharacterBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADMCharacterBase::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

float ADMCharacterBase::MontagePlay(UAnimMontage* IN InMontage, const FString SectionName /*= ""*/, float IN InPlayRate /*= 1.f*/)
{
	UDMPlayerAnimInstance* AnimInstance = Cast<UDMPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return 0.f;

	float fReturnValue = AnimInstance->Montage_Play(InMontage, InPlayRate);
	if (SectionName.IsEmpty() == false)
	{
		AnimInstance->Montage_JumpToSection(*SectionName);
	}
	return fReturnValue;
}

float ADMCharacterBase::MontagePlay(FName IN InRowName)
{
	FDMMontageTable* MontageTable = DMCharacterManager::Get()->GetMontageTable(InRowName);
	if (MontageTable)
	{
		return MontagePlay(MontageTable->Montage, MontageTable->SectionName);
	}
	return 0.f;
}

void ADMCharacterBase::OnMontageEnded(UAnimMontage* InMontage, bool InbInterrupted)
{
	UDMComponentFSM* FSMComponent = GetComponent<UDMComponentFSM>();
	if (FSMComponent)
	{
		return FSMComponent->OnMontageEnded(InMontage, InbInterrupted);
	}
}

bool ADMCharacterBase::DetermineFSM(const FDMFSMData IN InFSMData)
{
	UDMComponentFSM* FSMComponent = GetComponent<UDMComponentFSM>();
	if (FSMComponent)
	{
		return FSMComponent->DetermineFSM(InFSMData);
	}
	return false;
}

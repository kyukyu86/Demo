// Fill out your copyright notice in the Description page of Project Settings.


#include "DMCharacterMine.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "../../../Component/DMActorComponentFSM.h"
#include "../../../Manager/DMUIManager.h"
#include "../../../Manager/DMCharacterManager.h"
#include "../../../Manager/DMInputManager.h"
#include "../../../Component/DMActorComponentInteraction.h"
#include "../../FunctionComponent/DMInteractionComponent.h"



ADMCharacterMine::ADMCharacterMine() : ADMCharacterPlayer()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//
	UCapsuleComponent* CastedRootComponent = Cast<UCapsuleComponent>(RootComponent);
	if (CastedRootComponent)
	{
		CastedRootComponent->SetCollisionProfileName(FName("CharacterMine"));
	}
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(FName("CharacterMine"));
	}
}

void ADMCharacterMine::BeginPlay()
{
	Super::BeginPlay();

}

void ADMCharacterMine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{


	Super::EndPlay(EndPlayReason);
}

void ADMCharacterMine::BuildCustomComponents()
{
	Super::BuildCustomComponents();


}

//////////////////////////////////////////////////////////////////////////
// Input

#define BIND_CUSTOM_INPUT(_Key)	\
PlayerInputComponent->BindAction<FDMBindingInputDelegate>(#_Key, EInputEvent::IE_Pressed, this, &ADMCharacterMine::OnInputEvent, EDMInput::_Key, EInputEvent::IE_Pressed);	\
PlayerInputComponent->BindAction<FDMBindingInputDelegate>(#_Key, EInputEvent::IE_Released, this, &ADMCharacterMine::OnInputEvent, EDMInput::_Key, EInputEvent::IE_Released);	\


void ADMCharacterMine::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADMCharacterMine::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADMCharacterMine::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADMCharacterMine::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADMCharacterMine::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADMCharacterMine::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADMCharacterMine::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADMCharacterMine::OnResetVR);

	// Temp Input
	BIND_CUSTOM_INPUT(Key_1);
	BIND_CUSTOM_INPUT(Key_2);
	BIND_CUSTOM_INPUT(Key_3);
	BIND_CUSTOM_INPUT(Key_4);
}

void ADMCharacterMine::SetTargetedComponent(UDMInteractionComponent* IN InTargetComponent)
{
	if (InTargetComponent && InTargetComponent->IsContainedFlag(EDMInteractionFunctionFlag::Targeting) == false)
		return;

	if (TargetedInteractionComponent != InTargetComponent)
	{
		if (TargetedInteractionComponent)
		{
			TargetedInteractionComponent->OnTargetReleased();
		}

		if (InTargetComponent)
		{
			InTargetComponent->OnTargeted();
		}
	}
	
	TargetedInteractionComponent = InTargetComponent;
}

void ADMCharacterMine::OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	if (DetermineInputEvent(InInputType, InEventType))
		return;

	if (DetermineCharacterInputEvent(InInputType, InEventType))
		return;	
}

bool ADMCharacterMine::DetermineInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	// 1. Check Interaction
	UDMActorComponentInteraction* ActorCompInteraction = GetComponent<UDMActorComponentInteraction>();
	if (ActorCompInteraction && ActorCompInteraction->OnInputEvent(InInputType, InEventType))
	{
		return true;
	}

	return false;
}

bool ADMCharacterMine::DetermineCharacterInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	// ÀÏ´Ü.
	if (InEventType == EInputEvent::IE_Released)
		return false;

	switch (InInputType)
	{
	case EDMInput::Key_1:
	{
		if (strAsyncKeyForUI.IsEmpty() == false)
			return false;

		if (DMUIManager::Get()->IsOpenedPanel(EDMPanelKind::Menu))
		{
			DMUIManager::Get()->ClosePanel(EDMPanelKind::Menu);
		}
		else
		{
			FDMOpenWidgetInfo OpenWidgetInfo(EDMPanelKind::Menu, GetActorLocation());
			auto CompleteDeleagate = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
			{
				strAsyncKeyForUI.Empty();
			});
			OpenWidgetInfo.CompleteDelegate = CompleteDeleagate;
			strAsyncKeyForUI = DMUIManager::Get()->OpenPanel(OpenWidgetInfo);
		}
	}
	break;
	case EDMInput::Key_2:
	{

	}
	break;
	case EDMInput::Key_3:
	{

	}
	break;
	case EDMInput::Key_4:
	{

	}
	break;
	}

	return false;
}

//====================================================================================

void ADMCharacterMine::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADMCharacterMine::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ADMCharacterMine::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}
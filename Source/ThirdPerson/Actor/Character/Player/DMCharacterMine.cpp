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
#include <Components/WidgetInteractionComponent.h>
#include "../../../GameInstance/DMGameInstance.h"
#include "../../../Util/DMUIUtil.h"



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

	// Set Collision Profile
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
	if (WidgetInteractionComponent)
	{
		WidgetInteractionComponent->UnregisterComponent();
		WidgetInteractionComponent->RemoveFromRoot();
		WidgetInteractionComponent->DestroyComponent();
	}

	Super::EndPlay(EndPlayReason);
}

void ADMCharacterMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateWidgetInteractionByMouseCursorPosition();
}

void ADMCharacterMine::BuildCustomComponents()
{
	Super::BuildCustomComponents();

	WidgetInteractionComponent = NewObject<UWidgetInteractionComponent>();
	WidgetInteractionComponent->AddToRoot();
	//WidgetInteractionComponent->RegisterComponent();
	WidgetInteractionComponent->RegisterComponentWithWorld(GetWorld());
	WidgetInteractionComponent->InteractionDistance = 2000.f;
	//WidgetInteractionComponent->bShowDebug = true;
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
	PlayerInputComponent->BindAxis("Turn", this, &ADMCharacterMine::TurnAtRateForMouse);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADMCharacterMine::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ADMCharacterMine::LookUpAtRateForMouse);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADMCharacterMine::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADMCharacterMine::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADMCharacterMine::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADMCharacterMine::OnResetVR);

	// Mouse
	PlayerInputComponent->BindAxis("MouseWheel", this, &ADMCharacterMine::MouseWheelRate);

	// Temp Input
	BIND_CUSTOM_INPUT(Key_1);
	BIND_CUSTOM_INPUT(Key_2);
	BIND_CUSTOM_INPUT(Key_3);
	BIND_CUSTOM_INPUT(Key_4);
	BIND_CUSTOM_INPUT(MouseRButton);
	BIND_CUSTOM_INPUT(MouseLButton);
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
	switch (InInputType)
	{
	case EDMInput::Key_1:
	{
		// ÀÏ´Ü.
		if (InEventType == EInputEvent::IE_Released)
			return false;

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

	case EDMInput::MouseRButton:
	{
		bIsMouseRButtonPressed = InEventType == EInputEvent::IE_Pressed ? true : false;
	}
	break;
	case EDMInput::MouseLButton:
	{
		if (WidgetInteractionComponent)
		{
			if (InEventType == EInputEvent::IE_Pressed)
			{
				WidgetInteractionComponent->PressPointerKey(EKeys::LeftMouseButton);
			}
			else
			{
				WidgetInteractionComponent->ReleasePointerKey(EKeys::LeftMouseButton);
			}
		}
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

void ADMCharacterMine::TurnAtRateForMouse(float Rate)
{
	if (bIsMouseRButtonPressed == false)
		return;

	APawn::AddControllerYawInput(Rate);
}

void ADMCharacterMine::LookUpAtRateForMouse(float Rate)
{
	if (bIsMouseRButtonPressed == false)
		return;

	APawn::AddControllerPitchInput(Rate);
}

void ADMCharacterMine::TurnAtRate(float Rate)
{
	ADMCharacterBase::TurnAtRate(Rate);
}

void ADMCharacterMine::LookUpAtRate(float Rate)
{
	ADMCharacterBase::LookUpAtRate(Rate);
}

void ADMCharacterMine::MouseWheelRate(float Rate)
{
	if (WidgetInteractionComponent)
	{
		WidgetInteractionComponent->ScrollWheel(Rate);
	}
}

void ADMCharacterMine::UpdateWidgetInteractionByMouseCursorPosition()
{
	FVector MouseWorldLocation, MouseWorldDirection;
	DMUIUtil::GetMouseWorldTransform(MouseWorldLocation, MouseWorldDirection);
	WidgetInteractionComponent->SetWorldLocation(MouseWorldLocation);
	WidgetInteractionComponent->SetWorldRotation(MouseWorldDirection.Rotation());
}

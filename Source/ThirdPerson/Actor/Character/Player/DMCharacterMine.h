// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMCharacterPlayer.h"
#include "DMCharacterMine.generated.h"


DECLARE_DELEGATE_TwoParams(FDMBindingInputDelegate, EDMInput, EInputEvent);

class USpringArmComponent;
class UCameraComponent;
class UDMInteractionComponent;

UCLASS()
class THIRDPERSON_API ADMCharacterMine : public ADMCharacterPlayer
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float CustomizeMaxWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float CustomizeMaxSprintSpeed = 800.f;

	class UWidgetInteractionComponent* WidgetInteractionComponent = nullptr;
	UDMInteractionComponent* TargetedInteractionComponent = nullptr;
	bool bIsMouseRButtonPressed = false;

public:
	ADMCharacterMine();

private:
	bool DetermineInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);
	bool DetermineCharacterInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);

	void OnSpawned();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BuildCustomComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void TurnAtRate(float Rate) override;
	virtual void LookUpAtRate(float Rate) override;
	void OnResetVR();
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);	
	void TurnAtRateForMouse(float Rate);
	void LookUpAtRateForMouse(float Rate);	
	void MouseWheelRate(float Rate);

	void UpdateWidgetInteractionByMousePosition();

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetTargetedComponent(UDMInteractionComponent* IN InTargetComponent);
	UDMInteractionComponent* GetTargetedComponent() { return TargetedInteractionComponent; }

	void OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType);
};

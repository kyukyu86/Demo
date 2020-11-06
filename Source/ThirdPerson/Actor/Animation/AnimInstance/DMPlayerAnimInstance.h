// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DMPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = DMPlayerAnimInstance, meta = (AllowPrivateAccess = "true"))
		bool bRun = false;

public:
	virtual void NativeBeginPlay() override;

	void SetRun(const bool IN InSet) { bRun = InSet; }
};

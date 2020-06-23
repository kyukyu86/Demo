// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMGameModeBase.h"
#include "DMGameModeField.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API ADMGameModeField : public ADMGameModeBase
{
	GENERATED_BODY()
	
public:
	ADMGameModeField();

public:
	virtual void StartPlay() override;
};
